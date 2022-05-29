#include "boost/thread.hpp"
#include "engine/log.h"
#include "engine/rpc_manager.h"
#include "engine/gvalue.h"
#include "engine/ini.h"

using namespace std;

extern void engine_init(GString ip, uint16_t port);
extern void engine_tick();

extern void game_rpc_handle_register();

void init(int argc, char* args[]) {

    const char* ini_file = "game.ini";
    g_ini.parser_ini(ini_file);

    auto ip = g_ini.get_string("Listen", "ip");
    auto port = g_ini.get_int("Listen", "port");
    engine_init(ip, port);

    game_rpc_handle_register();
}

int main(int argc, char* args[]) {

    init(argc, args);

    INFO_LOG("main tick start\n");

    // main tick
    while (true) {

        engine_tick();

        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    INFO_LOG("end\n");
    return 0;
}