#include <string>

#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"

#include "boost_asio.h"
#include "log.h"
#include "rpc_manager.h"

using boost::asio::ip::tcp;

void Session::start() {
    do_read();
}

void Session::do_read() {
    // �������ü���������Session���ͷ�
    auto self(shared_from_this());

    m_socket.async_read_some(
        boost::asio::buffer(m_buffer),
        [this, self](boost::system::error_code ec, size_t length) {
        on_read(ec, length);
    });
}

void Session::on_read(boost::system::error_code ec, size_t length) {

    if (!ec) {
        auto _id = boost::this_thread::get_id();
        string thread_id = boost::lexical_cast<string>(_id);

        auto addr = m_socket.remote_endpoint().address().to_string();
        INFO_LOG("thread.%s on_read: length.%d from %s\n", thread_id.c_str(), int(length), get_remote_addr().c_str());

        // ճ��
        memcpy(m_buffer_cache + m_cache_idx, m_buffer, length);
        m_cache_idx += length;

        // �����л�
        auto process_len = g_rpc_manager.rpc_imp_generate(m_buffer_cache, m_cache_idx);

        // ���
        memcpy(m_buffer_cache, m_buffer_cache + process_len, m_cache_idx - process_len);
        m_cache_idx -= process_len;

    } else {
        if (ec == boost::asio::error::eof) {
            ERROR_LOG("socket read eof: %s\n", ec.message().c_str());
        }
        else if (ec == boost::asio::error::operation_aborted) {
            ERROR_LOG("socket operation aborted: %s\n", ec.message().c_str());
        }
        else {
            ERROR_LOG("socket read error: %s\n", ec.message().c_str());
        }
        return;
    }

    do_read();
}

void Session::do_write(char *buffer, size_t length) {

    boost::asio::async_write(
        m_socket,
        boost::asio::buffer(buffer, length),
        [this](boost::system::error_code ec, size_t length) {
        if (ec) {
            ERROR_LOG("socket write error %s\n", ec.message().c_str());
        }
    });
}

string Session::get_remote_addr() {
    return m_socket.remote_endpoint().address().to_string();
}

// -----------------------------------------------------------------------------

void Server::do_accept() {
    m_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            // TODO - Session����Session�Ľӿڶ����̰߳�ȫ�ģ����߳̿���ֱ��ʹ��
            make_shared<Session>(move(socket))->start();
        }
        else {
            ERROR_LOG("accept %s", ec.message().c_str());
        }
    });
}

uint16_t port = 8090;
boost::asio::io_context io_context;
Server server{ io_context, port };

void boost_asio_init() {
    // ��run��do_accpet֮��run������
    io_context.run();
    INFO_LOG("boost asio init\n");
}

// boost_asio_tick��asio����tick���������Է������߳�Ҳ���������߳�
void boost_asio_tick() {

    server.do_accept();
    io_context.run();

    //INFO_LOG("tick\n");
}

// ���߳�boost_asio_tick
void boost_asio_thread() {
    while (true) {
        boost_asio_tick();
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
}

void boost_asio_start() {
    boost::thread t1(boost_asio_thread);
    boost::thread t2(boost_asio_thread);
    boost::thread t3(boost_asio_thread);
}
