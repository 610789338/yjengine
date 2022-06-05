#include <string>

#include "boost/thread.hpp"
#include "engine/log.h"
#include "engine/rpc_manager.h"
#include "engine/gvalue.h"
#include "engine/ini.h"
#include "engine/remote_manager.h"

using namespace std;

extern void set_engine_listen_ipport(GString ip, uint16_t port);
extern void engine_init();
extern void engine_tick();

extern void gate_rpc_handle_register();

void init(int argc, char* args[]) {

    const char* ini_file = "gate.ini";
    g_ini.parser_ini(ini_file);

    auto ip = g_ini.get_string("Listen", "ip");
    auto port = g_ini.get_int("Listen", "port");
    set_engine_listen_ipport(ip, port);
    
    engine_init();

    gate_rpc_handle_register();
}

void connect_game() {
    auto ip = g_ini.get_string("Game1", "ip");
    auto port = g_ini.get_string("Game1", "port");

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
