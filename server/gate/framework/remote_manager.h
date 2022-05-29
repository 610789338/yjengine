#pragma once
// Asynchronous echo client.

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <map>

#include "boost/asio.hpp"
#include "boost/core/ignore_unused.hpp"
#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"

#include "engine/gvalue.h"

using namespace std;
using boost::asio::ip::tcp;

// -----------------------------------------------------------------------------

class Remote : public std::enable_shared_from_this<Remote> {
public:
    Remote(boost::asio::io_context& io_context);

    void start(const GString& host, const GString& port);
    void close();

    GString get_local_addr();
    GString get_remote_addr();

    template<class ...T>
    void remote_rpc_call(GString rpc_name, const T&... args) {
        Encoder encoder = g_rpc_manager.rpc_encode(rpc_name, args...);

        auto self(shared_from_this());
        boost::asio::async_write(m_socket, boost::asio::buffer(encoder.get_buf(), encoder.get_offset()),
            [this, self](boost::system::error_code ec, std::size_t length) {
            on_write(ec, length);
        });
    }

private:
    void on_resolve(boost::system::error_code ec, tcp::resolver::results_type endpoints);
    void on_connect(boost::system::error_code ec, tcp::endpoint endpoint, shared_ptr<Remote> self);

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
};

class RemoteManager {
public:
    RemoteManager() {}
    ~RemoteManager() {}

    void connect_remote(GString ip, GString port);
    void on_remote_connected(shared_ptr<Remote> remote);
    void on_remote_disconnected(GString remote_addr);

    void add_remote(shared_ptr<Remote> remote);
    void remove_remote(GString remote_addr);
    shared_ptr<Remote> get_remote(GString remote_addr);

private:
    shared_mutex m_mutex;
    map<GString, shared_ptr<Remote>> m_remotes;
};

extern RemoteManager g_remote_mgr;
