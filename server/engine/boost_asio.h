#pragma once

#include "boost/asio.hpp"
#include "boost/thread.hpp"

#include "gvalue.h"
#include "rpc_manager.h"
#include "utils.h"

using namespace std;
using boost::asio::ip::tcp;
using boost::asio::ip::address;

extern boost::asio::io_context g_io_context;

// Session
class Session : public std::enable_shared_from_this<Session> {

public:
    Session(tcp::socket socket) : m_socket(std::move(socket)) {}
    ~Session() {}

    void start();
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

        Encoder encoder = std::move(remote_rpc_queue_ele->encode());
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
        Encoder encoder = std::move(g_rpc_manager.rpc_encode(rpc_name, args...));
        //Decoder decoder(encoder.get_buf(), encoder.get_offset());
        //auto pkg_len = decoder.read_int16();
        //auto imp = g_rpc_manager.rpc_decode(decoder.get_buf() + decoder.get_offset(), pkg_len);
        auto imp = g_rpc_manager.rpc_decode(encoder.get_buf() + 2, encoder.get_offset() - 2);
        imp->set_session(shared_from_this());
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

    bool get_verify() { return m_is_verify; }
    void set_verify(bool success) { m_is_verify = success; }

    int64_t get_last_active_time() { return m_last_active_time; }
    int64_t get_next_heartbeat_time() { return m_next_heartbeat_time; }
    void set_next_heartbeat_time(int64_t next_heartbeat_time);

private:
    void on_write(boost::system::error_code ec, std::size_t length);

    void do_read();
    void on_read(boost::system::error_code ec, std::size_t length);

    tcp::socket m_socket;

    enum {
        RCV_BUF = 16 * 1024,
        BUF_CACHE = 100 * 1024
    };

    char m_buffer[RCV_BUF] = {0};
    char m_buffer_cache[BUF_CACHE] = {0};
    short m_cache_idx = 0;

    GString m_remote_addr;
    GString m_local_addr;

    bool m_is_verify = false;
    int64_t m_last_active_time = 0;
    int64_t m_next_heartbeat_time = 0;
    boost::shared_mutex m_mutex;
};

class SessionManager {
    typedef std::function<void(const GString&, shared_ptr<Session>)> ForEachFunc;
public:
    SessionManager() {}
    ~SessionManager() {}

    void on_session_connected(const shared_ptr<Session>& session);
    void on_session_disconnected(const GString& session_addr);

    void add_session(const shared_ptr<Session>& session);
    void remove_session(const GString& session_addr);
    bool is_valid_session(const shared_ptr<Session>& session);

    shared_ptr<Session> get_session(const GString& session_addr);
    shared_ptr<Session> get_rand_session();
    shared_ptr<Session> get_fixed_session(const GString& input);
    void foreach_session(ForEachFunc func);

    // partial for gate
    void add_gate(const GString& session_addr, const GString& gate_addr);
    void remove_gate(const GString& session_addr);
    shared_ptr<Session> get_gate(const GString& gate_addr);

private:
    boost::shared_mutex m_mutex;
    unordered_map<GString, shared_ptr<Session>> m_sessions;
    unordered_map<shared_ptr<Session>, GString> m_sessions_turn;

    // gate addr必须是集群唯一的，所以要用gate listen addr，不能用session remote addr
    unordered_map<GString, GString> m_gates;  // gate addr/session addr
    unordered_map<GString, GString> m_gates_turn; // session addr/gate addr
};


// -----------------------------------------------------------------------------

class Server {

public:
    Server(boost::asio::io_context& io_context, const char* _ip, const uint16_t& port);
    ~Server() {}

    void do_accept();

    GString& get_listen_addr();

private:
    tcp::acceptor m_acceptor;
    GString m_listen_addr = "";
};

extern boost::asio::io_context g_io_context;
extern void boost_asio_init();
extern void boost_asio_start();
extern void boost_asio_tick();

extern std::shared_ptr<Server> server;
extern SessionManager g_session_mgr;
