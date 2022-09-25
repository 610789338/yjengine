#pragma once

#include "boost/asio.hpp"
#include "boost/thread.hpp"

#include "gvalue.h"
#include "rpc_manager.h"

using namespace std;
using boost::asio::ip::tcp;
using boost::asio::ip::address;

// Session
class Session : public std::enable_shared_from_this<Session> {

public:
    Session(tcp::socket socket) : m_socket(std::move(socket)) {}
    ~Session() {}

    void start();
    void close();

    GString get_local_addr();
    GString get_remote_addr();

    template<class ...T>
    void remote_rpc_call(const GString& rpc_name, const T&... args) {
        const Encoder& encoder = g_rpc_manager.rpc_encode(rpc_name, args...);
        auto self(shared_from_this());

        boost::asio::async_write(
            m_socket,
            boost::asio::buffer(encoder.get_buf(), encoder.get_offset()),
            [this, self](boost::system::error_code ec, std::size_t length) {
            on_write(ec, length);
        });
    }

    template<class... T>
    void local_rpc_call(const GString& rpc_name, const T&... args) {
        GArray rpc_params;
        args2array(rpc_params, args...);
        auto imp = make_shared<RpcImp>(rpc_name, rpc_params);
        imp->set_session(shared_from_this());
        g_rpc_manager.imp_queue_push(imp);
    }

    bool get_verify() { return is_verify; }
    void set_verify(bool success) { is_verify = success; }

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

    bool is_verify = false;
};

class SessionManager {
public:
    SessionManager() {}
    ~SessionManager() {}

    void on_session_connected(const shared_ptr<Session>& session);
    void on_session_disconnected(const GString& session_addr);

    void add_session(const shared_ptr<Session>& session);
    void remove_session(const GString& session_addr);
    shared_ptr<Session> get_session(const GString& session_addr);
    shared_ptr<Session> get_rand_session();

private:
    boost::shared_mutex m_mutex;
    unordered_map<GString, shared_ptr<Session>> m_sessions;
};


// -----------------------------------------------------------------------------

class Server {

public:
    Server(boost::asio::io_context& io_context, const char* _ip, const uint16_t& port)
        : m_acceptor(io_context, tcp::endpoint(address::from_string(_ip), port)) {}
    ~Server() {}

    void do_accept();

    GString get_listen_addr();

private:
    tcp::acceptor m_acceptor;
};

extern boost::asio::io_context io_context;
extern void boost_asio_init();
extern void boost_asio_start();
extern void boost_asio_tick();

extern std::shared_ptr<Server> server;
extern SessionManager g_session_mgr;
