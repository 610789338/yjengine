#include "engine/gvalue.h"
#include "engine/log.h"
#include "engine/rpc_manager.h"
#include "engine/boost_asio.h"
#include "engine/ini.h"

#include "remote_manager.h"

using namespace std;

RemoteManager g_remote_mgr;

Remote::Remote(boost::asio::io_context& io_context) : m_socket(io_context), m_resolver(io_context) {}

void Remote::start(const GString& host, const GString& port) {
    auto self(shared_from_this());
    m_resolver.async_resolve(tcp::v4(), host, port,
        [this, self](boost::system::error_code ec, tcp::resolver::results_type endpoints) {
        on_resolve(ec, endpoints);
    });
}

void Remote::on_resolve(boost::system::error_code ec, tcp::resolver::results_type endpoints) {
    if (ec) {
        ERROR_LOG("resolve error: %s\n", ec.message().c_str());
        return;
    }

    auto self(shared_from_this());
    boost::asio::async_connect(m_socket, endpoints,
        [this, self](boost::system::error_code ec, tcp::endpoint endpoints) {
        on_connect(ec, endpoints, self);
    });
}

void Remote::on_connect(boost::system::error_code ec, tcp::endpoint endpoint, shared_ptr<Remote> self) {
    if (ec) {
        ERROR_LOG("Connect failed: %s\n", ec.message().c_str());
        close();
        return;
    }

    g_remote_mgr.on_remote_connected(self);

    do_read();
}

void Remote::do_read() {
    auto self(shared_from_this());

    m_socket.async_read_some(
        boost::asio::buffer(m_buffer),
        [this, self](boost::system::error_code ec, size_t length) {
        on_read(ec, length);
    });
}

void Remote::on_read(boost::system::error_code ec, std::size_t length) {
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

        g_remote_mgr.on_remote_disconnected(get_remote_addr());
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
    auto process_len = g_rpc_manager.rpc_imp_generate(m_buffer_cache, m_cache_idx, nullptr);

    // 拆包
    memmove(m_buffer_cache, m_buffer_cache + process_len, m_cache_idx - process_len);
    m_cache_idx -= process_len;

    do_read();
}

void Remote::close() {
    m_socket.close();
}

GString Remote::get_local_addr() {
    auto ip = m_socket.local_endpoint().address().to_string();
    auto port = m_socket.local_endpoint().port();
    return ip + ":" + std::to_string(port);
}

GString Remote::get_remote_addr() {
    auto ip = m_socket.remote_endpoint().address().to_string();
    auto port = m_socket.remote_endpoint().port();
    return ip + ":" + std::to_string(port);
}

void Remote::on_write(boost::system::error_code ec, std::size_t length) {
    if (ec) {
        ERROR_LOG("write error : %s\n", ec.message().c_str());
    }
}

// ------------------------------ rpc manager ------------------------------

void RemoteManager::connect_remote(GString ip, GString port) {
    make_shared<Remote>(io_context)->start(ip, port);
}

void RemoteManager::on_remote_connected(shared_ptr<Remote> remote) {
    INFO_LOG("on_remote_connected %s\n", remote->get_remote_addr().c_str());

    add_remote(remote);

    REMOTE_RPC_CALL(remote, "register_from_gate", server->get_listen_addr());
}

void RemoteManager::on_remote_disconnected(GString remote_addr) {
    INFO_LOG("on_remote_disconnected %s\n", remote_addr.c_str());
    remove_remote(remote_addr);
}

void RemoteManager::add_remote(shared_ptr<Remote> remote) {
    unique_lock<shared_mutex> lock(m_mutex);
    m_remotes.insert(make_pair(remote->get_remote_addr(), remote));
}

void RemoteManager::remove_remote(GString remote_addr) {
    unique_lock<shared_mutex> lock(m_mutex);
    m_remotes.erase(remote_addr);
}

shared_ptr<Remote> RemoteManager::get_remote(GString remote_addr) {
    shared_lock<shared_mutex> lock(m_mutex);
    auto iter = m_remotes.find(remote_addr);
    if (iter == m_remotes.end()) {
        return nullptr;
    }

    return iter->second;
}

void client_disconnected(GString client_addr) {
}