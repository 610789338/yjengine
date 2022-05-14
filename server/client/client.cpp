// Asynchronous echo client.

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "boost/asio.hpp"
#include "boost/core/ignore_unused.hpp"
#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"
#include "engine/encode.h"

using namespace std;
using boost::asio::ip::tcp;

std::mutex g_log_mtx;
#define thread_safe_print(format, ...) {std::unique_lock<std::mutex> lock(g_log_mtx); printf(format, __VA_ARGS__);}

// -----------------------------------------------------------------------------

class Client {
public:
    Client(boost::asio::io_context& io_context, const std::string& host, const std::string& port);

private:
    void OnResolve(boost::system::error_code ec, tcp::resolver::results_type endpoints);
    void OnConnect(boost::system::error_code ec, tcp::endpoint endpoint);

    void DoWrite();
    void OnWrite(boost::system::error_code ec, std::size_t length);

    void OnRead(boost::system::error_code ec, std::size_t length);

    tcp::socket socket_;
    tcp::resolver resolver_;

    enum { BUF_SIZE = 1024 };

    // NOTE: std::vector is better than std::array in practice.
    std::array<char, BUF_SIZE> buf_;
};

// -----------------------------------------------------------------------------

Client::Client(boost::asio::io_context& io_context, const std::string& host, const std::string& port)
    : socket_(io_context), resolver_(io_context) {

    resolver_.async_resolve(tcp::v4(), host, port, std::bind(&Client::OnResolve, this, std::placeholders::_1, std::placeholders::_2));
}

void Client::OnResolve(boost::system::error_code ec, tcp::resolver::results_type endpoints) {
    if (ec) {
        thread_safe_print("Resolve: %s\n", ec.message().c_str());
    }
    else {
        // ConnectHandler: void(boost::system::error_code, tcp::endpoint)
        boost::asio::async_connect(socket_, endpoints, std::bind(&Client::OnConnect, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void Client::OnConnect(boost::system::error_code ec, tcp::endpoint endpoint) {
    if (ec) {
        thread_safe_print("Connect failed: %s\n", ec.message().c_str());
        socket_.close();
    }
    else {
        DoWrite();
    }
}

void Client::DoWrite() {

    Encoder encoder;
    encoder.write_string("rpc_normal_param_test");
    encoder.write_int8(-1);
    encoder.write_int16(-2);
    encoder.write_int32(-3);
    encoder.write_int64(0);
    encoder.write_uint8(-1);
    encoder.write_uint16(-2);
    encoder.write_uint32(-3);
    encoder.write_uint64(-4);
    encoder.write_float(123456789.123456789f);
    encoder.write_double(123456789123456789.123456789123456789);
    encoder.write_string("abc");
    encoder.write_end();

    thread_safe_print("offset %d\n", encoder.get_pkg_len());
    boost::asio::async_write(socket_, boost::asio::buffer(encoder.get_buf(), encoder.get_pkg_len()), std::bind(&Client::OnWrite, this, std::placeholders::_1, std::placeholders::_2));
}

void Client::OnWrite(boost::system::error_code ec, std::size_t length) {
    boost::ignore_unused(length);

    if (!ec) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        DoWrite();
        // socket_.async_read_some(boost::asio::buffer(buf_), std::bind(&Client::OnRead, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void Client::OnRead(boost::system::error_code ec, std::size_t length) {
    if (!ec) {
        auto _id = boost::this_thread::get_id();
        std::string thread_id = boost::lexical_cast<std::string>(_id);
        thread_safe_print("Reply is: %s.%s\n", buf_.data(), thread_id.c_str());
    }

    // Optionally, continue to write.
    // boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    DoWrite();

    // 频繁的关闭，会有问题，暂时还没弄清楚是为啥，先不管了，现实情况也不会这样频繁的开关
    // socket_.shutdown(tcp::socket::shutdown_both, ec);
    // socket_.close(ec);
}

// -----------------------------------------------------------------------------
// g++ boost_client.cpp -I /home/youjun/boost_1_75_0 -L /home/youjun/boost_1_75_0/build/lib -std=c++17 -lpthread -lboost_thread -g -o client

boost::asio::io_context io_context;

void thread_work() {
    // std::string host = "10.229.70.205";
    std::string host = "192.168.0.103";
    std::string port = "8090";

    Client client{ io_context, host, port };

    while (true) {
        io_context.run();
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }
}

int main(int argc, char* argv[]) {

    int pool_size = 1;

    std::vector<boost::thread*> thread_vec;
    for (size_t i = 0; i < pool_size; ++i) {
        boost::thread t(thread_work);
        thread_vec.push_back(&t);
    }

    for (size_t i = 0; i < pool_size; ++i) {
        thread_vec[i]->join();
    }

    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }

    return 0;
}
