#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>

#include "boost/asio.hpp"
#include "boost/core/ignore_unused.hpp"
#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"
#include "boost_asio.h"

#include "gvalue.h"
#include "utils.h"

using namespace std;
using boost::asio::ip::tcp;

extern boost::asio::io_context g_io_context;

// ----------------------------------------------------------------------------- //

class Remote : public std::enable_shared_from_this<Remote> {
public:
    Remote(boost::asio::io_context& io_context);

    void start(const GString& host, const GString& port);
    void close();

    GString& get_local_addr();
    GString& get_remote_addr();

    boost::shared_mutex m_queue_mutex;
    std::queue<shared_ptr<RemoteRpcQueueEleBase>> m_remote_rpc_queue;
    bool in_async_write = false;

    void push_remote_rpc_ele_to_queue(shared_ptr<RemoteRpcQueueEleBase>& remote_queue_rpc_ele);
    shared_ptr<RemoteRpcQueueEleBase> pop_remote_rpc_ele_from_queue();

    template<class ...T>
    void remote_rpc_call(const GString& rpc_name, const T&... args) {
        auto remote_queue_rpc_ele = g_remote_rpc_queue_ele_mgr.gen_remote_rpc_queue_ele(rpc_name, args...);

        unique_lock<boost::shared_mutex> lock(m_queue_mutex);
        push_remote_rpc_ele_to_queue(remote_queue_rpc_ele);
        if (!in_async_write) {
            in_async_write = true;
            auto self(shared_from_this());
            g_io_context.post([this, self]() {
                async_write_one_ele();
            });
        }
    }

    void async_write_one_ele() {
        auto self(shared_from_this());

        shared_ptr<RemoteRpcQueueEleBase> remote_rpc_queue_ele = nullptr;
        {
            unique_lock<boost::shared_mutex> lock(m_queue_mutex);
            remote_rpc_queue_ele = pop_remote_rpc_ele_from_queue();
            if (!remote_rpc_queue_ele) {
                in_async_write = false;
                return;
            }
        }

        const Encoder& encoder = remote_rpc_queue_ele->encode();
        boost::asio::async_write(
            m_socket,
            boost::asio::buffer(encoder.get_buf(), encoder.get_offset()),
            [this, self](boost::system::error_code ec, std::size_t length) {
                on_write(ec, length);
                async_write_one_ele();
            });

        set_next_heartbeat_time(nowms_timestamp() + 1000);
    }

    //template<class ...T>
    //void remote_rpc_call(const GString& rpc_name, const T&... args) {
    //    const Encoder& encoder = g_rpc_manager.rpc_encode(rpc_name, args...);
    //    auto self(shared_from_this());

    //    boost::asio::async_write(
    //        m_socket, 
    //        boost::asio::buffer(encoder.get_buf(), encoder.get_offset()),
    //        [this, self](boost::system::error_code ec, std::size_t length) {
    //            on_write(ec, length);
    //        });

    //    set_next_heartbeat_time(nowms_timestamp() + 1000);
    //}

    template<class... T>
    void local_rpc_call(const GString& rpc_name, const T&... args) {
        const Encoder& encoder = g_rpc_manager.rpc_encode(rpc_name, args...);
        auto imp = g_rpc_manager.rpc_decode(encoder.get_buf() + 2, encoder.get_offset() - 2);
        imp->set_remote(shared_from_this());
        g_rpc_manager.imp_queue_push(imp);
    }

    void send_buff_thread_safe(const Encoder& encoder) {
        auto self(shared_from_this());
        boost::asio::async_write(
            m_socket,
            boost::asio::buffer(encoder.get_buf(), encoder.get_offset()),
            [this, self](boost::system::error_code ec, std::size_t length) {
                on_write(ec, length);
            });
    }

    int64_t get_last_active_time() { return m_last_active_time; }
    int64_t get_next_heartbeat_time() { return m_next_heartbeat_time; }
    void set_next_heartbeat_time(int64_t next_heartbeat_time);

private:
    void on_resolve(boost::system::error_code ec, tcp::resolver::results_type endpoints);
    void on_connect(boost::system::error_code ec, tcp::endpoint endpoint);

    void on_write(boost::system::error_code ec, std::size_t length);

    void do_read();
    void on_read(boost::system::error_code ec, std::size_t length);

    tcp::socket m_socket;
    tcp::resolver m_resolver;

    enum {
        RCV_BUF = 16 * 1024,
        BUF_CACHE = 100 * 1024
    };

    char m_buffer[RCV_BUF] = { 0 };
    char m_buffer_cache[BUF_CACHE] = { 0 };
    short m_cache_idx = 0;

    GString m_remote_addr;
    GString m_local_addr;

    int64_t m_last_active_time = 0;
    int64_t m_next_heartbeat_time = 0;
    boost::shared_mutex m_mutex;
};

class RemoteManager {
    typedef std::function<void(const GString&, shared_ptr<Remote>)> ForEachFunc;
public:
    RemoteManager() {}
    ~RemoteManager() {}

    void connect_remote(const GString& ip, const GString& port);
    void on_remote_connected(const shared_ptr<Remote>& remote);
    void on_remote_disconnected(const GString& remote_addr);

    void add_remote(const shared_ptr<Remote>& remote);
    void remove_remote(const GString& remote_addr);
    bool is_valid_remote(const shared_ptr<Remote>& remote);

    shared_ptr<Remote> get_remote(const GString& remote_addr);
    shared_ptr<Remote> get_rand_remote();
    shared_ptr<Remote> get_rand_remote_exclude(const GString& exclude_addr);
    shared_ptr<Remote> get_fixed_remote(const GString& input);
    void foreach_remote(ForEachFunc func);

private:
    boost::shared_mutex m_mutex;
    unordered_map<GString, shared_ptr<Remote>> m_remotes;
    unordered_map<shared_ptr<Remote>, GString> m_remotes_turn;
};

extern RemoteManager g_remote_mgr;
