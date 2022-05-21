#pragma once

#include <string>

#include "boost/asio.hpp"

using namespace std;
using boost::asio::ip::tcp;

// Session
class Session : public std::enable_shared_from_this<Session> {

    enum {
        RCV_BUF     = 16 * 1024,
        BUF_CACHE   = 100 * 1024 
    };

public:
    Session(tcp::socket socket) : m_socket(std::move(socket)) {}
    ~Session() {}

    void start();
    string get_remote_addr();

private:
    void do_read();
    void on_read(boost::system::error_code ec, std::size_t length);
    void do_write(char *buffer, size_t length);

    tcp::socket m_socket;
    char m_buffer[RCV_BUF] = {0};
    char m_buffer_cache[BUF_CACHE] = {0};
    short m_cache_idx = 0;
};

// -----------------------------------------------------------------------------

class Server {

public:
    Server(boost::asio::io_context& io_context, uint16_t port)
        : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {}
    ~Server() {}

    void do_accept();

private:
    tcp::acceptor m_acceptor;
};

extern void boost_asio_init(uint16_t port);
extern void boost_asio_start();
extern void boost_asio_tick();
