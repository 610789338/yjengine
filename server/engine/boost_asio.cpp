#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"

#include "boost_asio.h"
#include "log.h"
#include "rpc_manager.h"
#include "utils.h"

using boost::asio::ip::tcp;

boost::asio::io_context io_context;

extern void client_disconnected(GString client_addr);

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

    if (ec) {
        if (ec == boost::asio::error::eof) {
            ERROR_LOG("socket read eof: %s\n", ec.message().c_str());
        }
        else if (ec == boost::asio::error::operation_aborted) {
            ERROR_LOG("socket operation aborted: %s\n", ec.message().c_str());
        }
        else {
            ERROR_LOG("socket read error: %s\n", ec.message().c_str());
        }

        client_disconnected(get_remote_addr());
        close();

        return;
    }

    //auto _id = boost::this_thread::get_id();
    //GString thread_id = boost::lexical_cast<GString>(_id);
    //INFO_LOG("thread.%s on_read: length.%d from %s\n", thread_id.c_str(), int(length), get_remote_addr().c_str());

    // 粘包
    memmove(m_buffer_cache + m_cache_idx, m_buffer, length);
    m_cache_idx += uint16_t(length);

    // 反序列化
    auto process_len = g_rpc_manager.rpc_imp_generate(m_buffer_cache, m_cache_idx, this);

    // 拆包
    memmove(m_buffer_cache, m_buffer_cache + process_len, m_cache_idx - process_len);
    m_cache_idx -= process_len;

    do_read();
}

void Session::on_write(boost::system::error_code ec, std::size_t length) {
    if (ec) {
        ERROR_LOG("write error : %s\n", ec.message().c_str());
    }
}

void Session::close() {
    m_socket.close();
}

GString Session::get_local_addr() {
    auto ip = m_socket.local_endpoint().address().to_string();
    auto port = m_socket.local_endpoint().port();
    return ip + ":" + std::to_string(port);
}

GString Session::get_remote_addr() {
    auto ip = m_socket.remote_endpoint().address().to_string();
    auto port = m_socket.remote_endpoint().port();
    return ip + ":" + std::to_string(port);
}

// -----------------------------------------------------------------------------

void Server::do_accept() {
    m_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            // TODO - Session管理，Session的接口都是线程安全的，主线程可以直接使用
            make_shared<Session>(std::move(socket))->start();
        }
        else {
            ERROR_LOG("accept %s", ec.message().c_str());
        }

        // 不用重复accept
        //do_accept();
    });
}

GString Server::get_listen_addr() {
    auto ip = m_acceptor.local_endpoint().address().to_string();
    auto port = m_acceptor.local_endpoint().port();
    return ip + ":" + std::to_string(port);
}

std::shared_ptr<Server> server = nullptr;


void boost_asio_init(GString ip, uint16_t port = 8090) {
    server = make_shared<Server>(io_context, ip.c_str(), port);

    // 先run，do_accpet之后run会阻塞
    io_context.run();
    INFO_LOG("boost asio init %d\n", port);
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
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

void boost_asio_start() {
    for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
        boost::thread t(boost_asio_thread);
    }
}
