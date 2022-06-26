#include "engine/engine.h"

#include "gameplay/game_utils.h"

using namespace std;

void init(int argc, char* args[]) {

    const char* ini_file = "game.ini";
    g_ini.parser_ini(ini_file);

    auto ip = g_ini.get_string("Listen", "ip");
    auto port = g_ini.get_int("Listen", "port");
    set_engine_listen_ipport(ip, port);

    engine_init();
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
