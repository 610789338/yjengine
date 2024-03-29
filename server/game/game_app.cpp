#include "engine/engine.h"

#include "gameplay/game_utils.h"

using namespace std;

bool g_is_server = true;

GString ini_file("game.ini");
extern void create_game_instance();

void init(int argc, char* args[]) {

    int idx = 0;
    while (idx < argc) {
        if (strcmp(args[idx], "-c") == 0) {
            ++idx;
            ini_file = args[idx];
        }

        ++idx;
    }

    // auto process_name = ini_get_string("Common", "name");
    // strcpy(args[0], process_name.c_str());

    auto ip = ini_get_string("Listen", "ip");
    auto port = ini_get_int("Listen", "port");
    set_engine_listen_ipport(ip, port);

    engine_init();
    create_game_instance();
}

extern void main_tick(const int64_t ms_pertick);

int main(int argc, char* args[]) {

    init(argc, args);

    main_tick(100);

    return 0;
}
