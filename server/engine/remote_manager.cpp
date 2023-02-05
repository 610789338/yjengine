#include "gvalue.h"
#include "log.h"
#include "rpc_manager.h"
#include "boost_asio.h"
#include "ini.h"

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
        on_connect(ec, endpoints);
    });
}

void Remote::on_connect(boost::system::error_code ec, tcp::endpoint endpoint) {
    if (ec) {
        ERROR_LOG("Connect failed: %s\n", ec.message().c_str());
        close();
        return;
    }

    LOCAL_RPC_CALL(shared_from_this(), "on_remote_connected");

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

        LOCAL_RPC_CALL(shared_from_this(), "on_remote_disconnected", get_remote_addr());
        close();

        return;
    }

    //auto _id = boost::this_thread::get_id();
    //GString thread_id = boost::lexical_cast<GString>(_id);
    //INFO_LOG("thread.%s on_read: length.%d from %s\n", thread_id.c_str(), int(length), get_remote_addr().c_str());

    m_last_active_time = nowms_timestamp(true);

    // 粘包
    memmove(m_buffer_cache + m_cache_idx, m_buffer, length);
    m_cache_idx += uint16_t(length);

    // 反序列化
    auto process_len = g_rpc_manager.rpc_imp_generate(m_buffer_cache, m_cache_idx, nullptr, shared_from_this());

    // 拆包
    memmove(m_buffer_cache, m_buffer_cache + process_len, m_cache_idx - process_len);
    m_cache_idx -= process_len;

    do_read();
}

void Remote::close() {
    m_socket.close();
}

GString& Remote::get_local_addr() {
    if (m_local_addr.empty()) {
        auto ip = m_socket.local_endpoint().address().to_string();
        auto port = m_socket.local_endpoint().port();
        m_local_addr = ip + ":" + std::to_string(port);
    }
    return m_local_addr;
}

GString& Remote::get_remote_addr() {
    if (m_remote_addr.empty()) {
        auto ip = m_socket.remote_endpoint().address().to_string();
        auto port = m_socket.remote_endpoint().port();
        m_remote_addr = ip + ":" + std::to_string(port);
    }
    return m_remote_addr;
}

void Remote::on_write(boost::system::error_code ec, std::size_t length) {
    if (ec) {
        ERROR_LOG("write error : %s\n", ec.message().c_str());

        LOCAL_RPC_CALL(shared_from_this(), "on_remote_disconnected", get_remote_addr());
    }
}

void Remote::set_next_heartbeat_time(int64_t next_heartbeat_time) {
    unique_lock<boost::shared_mutex> lock(m_mutex);
    if (m_next_heartbeat_time < next_heartbeat_time) {
        m_next_heartbeat_time = next_heartbeat_time;
    }
}

// ------------------------------ rpc manager ------------------------------

void RemoteManager::connect_remote(const GString& ip, const GString& port) {
    make_shared<Remote>(io_context)->start(ip, port);
}

void RemoteManager::on_remote_connected(const shared_ptr<Remote>& remote) {
    add_remote(remote);
}

void RemoteManager::on_remote_disconnected(const GString& remote_addr) {
    remove_remote(remote_addr);
}

void RemoteManager::add_remote(const shared_ptr<Remote>& remote) {
    unique_lock<boost::shared_mutex> lock(m_mutex);
    m_remotes.insert(make_pair(remote->get_remote_addr(), remote));
    m_remotes_turn.insert(make_pair(remote, remote->get_remote_addr()));
}

void RemoteManager::remove_remote(const GString& remote_addr) {
    unique_lock<boost::shared_mutex> lock(m_mutex);
    auto iter = m_remotes.find(remote_addr);
    if (iter == m_remotes.end()) {
        return;
    }
    m_remotes_turn.erase(iter->second);
    m_remotes.erase(iter);
}

bool RemoteManager::is_valid_remote(const shared_ptr<Remote>& remote) {
    return m_remotes_turn.find(remote) != m_remotes_turn.end();
}

shared_ptr<Remote> RemoteManager::get_remote(const GString& remote_addr) {
    shared_lock<boost::shared_mutex> lock(m_mutex);
    auto iter = m_remotes.find(remote_addr);
    if (iter == m_remotes.end()) {
        return nullptr;
    }

    return iter->second;
}

shared_ptr<Remote> RemoteManager::get_rand_remote() {
    shared_lock<boost::shared_mutex> lock(m_mutex);

    if (m_remotes.empty()) {
        return nullptr;
    }

    auto iter = m_remotes.begin();
    auto idx = rand() % m_remotes.size();
    for (; iter != m_remotes.end(); ++iter) {
        if (idx == 0) {
            return iter->second;
        }

        --idx;
    }

    return iter->second;
}

shared_ptr<Remote> RemoteManager::get_rand_remote_exclude(const GString& exclude_addr) {
    shared_lock<boost::shared_mutex> lock(m_mutex);

    if (m_remotes.empty()) {
        return nullptr;
    }

    if (m_remotes.size() == 1) {
        auto iter = m_remotes.begin();
        return iter->second;
    }

    unordered_map<GString, shared_ptr<Remote>> remotes_excluded;
    for (auto iter = m_remotes.begin(); iter != m_remotes.end(); ++iter) {
        if (iter->second->get_remote_addr() == exclude_addr) {
            continue;
        }

        remotes_excluded.insert(make_pair(iter->first, iter->second));
    }

    auto iter = remotes_excluded.begin();
    auto idx = rand() % remotes_excluded.size();
    for (; iter != remotes_excluded.end(); ++iter) {

        if (idx == 0) {
            return iter->second;
        }

        --idx;
    }

    return iter->second;
}

shared_ptr<Remote> RemoteManager::get_fixed_remote(const GString& input) {
    shared_lock<boost::shared_mutex> lock(m_mutex);

    if (m_remotes.empty()) {
        return nullptr;
    }

    auto idx = bkdr_hash(input.c_str()) % m_remotes.size();
    for (auto iter = m_remotes.begin(); iter != m_remotes.end(); ++iter) {
        if (idx == 0) {
            return iter->second;
        }

        --idx;
    }

    return nullptr;
}

void RemoteManager::foreach_remote(ForEachFunc func) {
    shared_lock<boost::shared_mutex> lock(m_mutex);
    for (auto iter = m_remotes.begin(); iter != m_remotes.end(); ++iter) {
        func(iter->first, iter->second);
    }
}
