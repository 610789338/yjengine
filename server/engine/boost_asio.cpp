﻿#include <stdlib.h>

#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"

#include "boost_asio.h"
#include "log.h"
#include "rpc_manager.h"
#include "utils.h"

using namespace std;
using boost::asio::ip::tcp;

boost::asio::io_context io_context;

SessionManager g_session_mgr;

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

        LOCAL_RPC_CALL(shared_from_this(), "disconnect_from_client", get_remote_addr());
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
    auto process_len = g_rpc_manager.rpc_imp_generate(m_buffer_cache, m_cache_idx, shared_from_this(), nullptr);

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

GString& Session::get_local_addr() {
    if (m_local_addr.empty()) {
        auto ip = m_socket.local_endpoint().address().to_string();
        auto port = m_socket.local_endpoint().port();
        m_local_addr = ip + ":" + std::to_string(port);
    }
    return m_local_addr;
}

GString& Session::get_remote_addr() {
    if (m_remote_addr.empty()) {
        auto ip = m_socket.remote_endpoint().address().to_string();
        auto port = m_socket.remote_endpoint().port();
        m_remote_addr = ip + ":" + std::to_string(port);
    }
    return m_remote_addr;
}

// -----------------------------------------------------------------------------

void SessionManager::on_session_connected(const shared_ptr<Session>& session) {
    add_session(session);
}

void SessionManager::on_session_disconnected(const GString& session_addr) {
    remove_session(session_addr);
}

void SessionManager::add_session(const shared_ptr<Session>& session) {
    //unique_lock<boost::shared_mutex> lock(m_mutex);
    m_sessions.insert(make_pair(session->get_remote_addr(), session));
    m_sessions_turn.insert(make_pair(session, session->get_remote_addr()));
}

void SessionManager::remove_session(const GString& session_addr) {
    //unique_lock<boost::shared_mutex> lock(m_mutex);
    auto iter = m_sessions.find(session_addr);
    m_sessions_turn.erase(iter->second);
    m_sessions.erase(iter);
}

bool SessionManager::is_valid_session(const shared_ptr<Session>& session) {
    return m_sessions_turn.find(session) != m_sessions_turn.end();
}

shared_ptr<Session> SessionManager::get_session(const GString& session_addr) {
    shared_lock<boost::shared_mutex> lock(m_mutex);
    
    auto iter = m_sessions.find(session_addr);
    if (iter == m_sessions.end()) {
        return nullptr;
    }

    return iter->second;
}

shared_ptr<Session> SessionManager::get_rand_session() {
    shared_lock<boost::shared_mutex> lock(m_mutex);

    if (m_sessions.empty()) {
        return nullptr;
    }

    auto idx = rand() % m_sessions.size();
    for (auto iter = m_sessions.begin(); iter != m_sessions.end(); ++iter) {
        if (idx == 0) {
            return iter->second;
        }
        --idx;
    }

    return nullptr;
}

shared_ptr<Session> SessionManager::get_fixed_session(const GString& input) {
    shared_lock<boost::shared_mutex> lock(m_mutex);

    if (m_sessions.empty()) {
        return nullptr;
    }

    auto idx = bkdr_hash(input.c_str()) % m_sessions.size();
    for (auto iter = m_sessions.begin(); iter != m_sessions.end(); ++iter) {
        if (idx == 0) {
            return iter->second;
        }
        --idx;
    }

    return nullptr;
}


// -----------------------------------------------------------------------------

void Server::do_accept() {
    m_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            // Session的接口都是线程安全的，主线程可以直接使用
            auto session = make_shared<Session>(std::move(socket));
            session->start();
            
            LOCAL_RPC_CALL(session, "connect_from_client");
        }
        else {
            ERROR_LOG("accept %s", ec.message().c_str());
        }

        do_accept();
    });
}

GString& Server::get_listen_addr() {
    if (m_listen_addr.empty()) {
        auto ip = m_acceptor.local_endpoint().address().to_string();
        auto port = m_acceptor.local_endpoint().port();
        m_listen_addr = ip + ":" + std::to_string(port);
    }
    return m_listen_addr;
}


GString listen_ip = "";
uint16_t listen_port = 0;
std::shared_ptr<Server> server = nullptr;

void set_engine_listen_ipport(GString ip, uint16_t port) {
    listen_ip = ip;
    listen_port = port;
}

void boost_asio_init() {
    if (!listen_ip.empty() && listen_port != 0) {
        server = make_shared<Server>(io_context, listen_ip.c_str(), listen_port);

        // 单线程要先run，do_accpet之后再run会阻塞
        // io_context.run();
        server->do_accept();

        INFO_LOG("boost asio listen@%s\n", IPPORT_STRING(listen_ip, listen_port).c_str());
    }
}

// boost_asio_tick是asio的主tick函数，可以放在主线程也可以在子线程
void boost_asio_tick() {

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
