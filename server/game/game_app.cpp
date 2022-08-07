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

        auto const tick_begin = nowms_timestamp(false);
        engine_tick();
        auto const tick_end = nowms_timestamp(false);

        //DEBUG_LOG("tick cost %lld.ms\n", tick_end - tick_begin);
        boost::this_thread::sleep(boost::posix_time::milliseconds(100 - (tick_end - tick_begin)));
    }

    INFO_LOG("end\n");
    return 0;
}
