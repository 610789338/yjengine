#include <string>

#include "engine/engine.h"

using namespace std;

const char* ini_file = "gate.ini";

void init(int argc, char* args[]) {

    auto ip = ini_get_string("Listen", "ip");
    auto port = ini_get_int("Listen", "port");
    set_engine_listen_ipport(ip, port);
    
    engine_init();
}

void connect_game() {
    auto ip = ini_get_string("Game1", "ip");
    auto port = ini_get_string("Game1", "port");

    INFO_LOG("connect game %s:%s\n", ip.c_str(), port.c_str());
    g_remote_mgr.connect_remote(ip, port);
}

int main(int argc, char* args[]) {

    init(argc, args);

     connect_game();

    INFO_LOG("main tick start\n");

    // main tick
    while (true) {

        engine_tick();

        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    INFO_LOG("end\n");
    return 0;
}
