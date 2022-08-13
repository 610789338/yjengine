#include <string>

#include "boost/thread.hpp"

#include "engine/engine.h"

#include "gameplay/entities/account.h"

using namespace std;

extern void set_engine_listen_ipport(GString ip, uint16_t port);
extern void engine_init();
extern void engine_tick();

const char* ini_file = "client.ini";

void init(int argc, char* args[]) {
    engine_init();
}

void connect_gate() {
    auto ip = ini_get_string("Gate", "ip");
    auto port = ini_get_string("Gate", "port");

    INFO_LOG("connect gate %s:%s\n", ip.c_str(), port.c_str());
    g_remote_mgr.connect_remote(ip, port);
}

int main(int argc, char* args[]) {

    init(argc, args);

    connect_gate();
    //connect_gate();
    //connect_gate();
    //connect_gate();
    //connect_gate();

    INFO_LOG("main tick start\n");

    // main tick
    while (true) {

        engine_tick();

        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    INFO_LOG("end\n");
    return 0;
}
