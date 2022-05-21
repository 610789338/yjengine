#include <string>

#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"

#include "boost_asio.h"
#include "log.h"
#include "rpc_manager.h"
#include "utils.h"

using boost::asio::ip::tcp;

void Session::start() {
    do_read();
}

void Session::do_read() {
    // 增加引用计数，避免Session被释放
    auto self(shared_from_this());

    m_socket.async_read_some(
        boost::asio::buffer(m_buffer),
        [this, self](boost::system::error_code ec, size_t length) {
        on_read(ec, length);
    });
}

void Session::on_read(boost::system::error_code ec, size_t length) {
    //byte_print(m_buffer, length);

    if (!ec) {
        auto _id = boost::this_thread::get_id();
        string thread_id = boost::lexical_cast<string>(_id);
        INFO_LOG("thread.%s on_read: length.%d from %s\n", thread_id.c_str(), int(length), get_remote_addr().c_str());

        // 粘包
        memmove(m_buffer_cache + m_cache_idx, m_buffer, length);
        m_cache_idx += uint16_t(length);

        // 反序列化
        auto process_len = g_rpc_manager.rpc_imp_generate(m_buffer_cache, m_cache_idx);

        // 拆包
		memmove(m_buffer_cache, m_buffer_cache + process_len, m_cache_idx - process_len);
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
            // TODO - Session管理，Session的接口都是线程安全的，主线程可以直接使用
            make_shared<Session>(move(socket))->start();
        }
        else {
            ERROR_LOG("accept %s", ec.message().c_str());
        }
    });
}

boost::asio::io_context io_context;
std::shared_ptr<Server> server = nullptr;


void boost_asio_init(uint16_t port = 8090) {
    server = make_shared<Server>(io_context, port);

    // 先run，do_accpet之后run会阻塞

    io_context.run();
    INFO_LOG("boost asio init\n");
}

// boost_asio_tick是asio的主tick函数，可以放在主线程也可以在子线程
void boost_asio_tick() {

    server->do_accept();
    io_context.run();

    //INFO_LOG("tick\n");
}

// 子线程boost_asio_tick
void boost_asio_thread() {
    while (true) {
        boost_asio_tick();
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
}

void boost_asio_start() {
    for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
        boost::thread t(boost_asio_thread);
    }
}
