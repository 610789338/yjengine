#include <string>

#include "engine/engine.h"

using namespace std;

bool g_is_server = true;

GString ini_file("gate.ini");
extern void create_gate_instance();

void init(int argc, char* args[]) {

    int idx = 0;
    while (idx < argc) {
        if (strcmp(args[idx], "-c") == 0) {
            ++idx;
            ini_file = args[idx];
        }

        ++idx;
    }

    auto ip = ini_get_string("Listen", "ip");
    auto port = ini_get_int("Listen", "port");
    set_engine_listen_ipport(ip, port);
    
    engine_init();
    create_gate_instance();
}

void connect_game() {

    auto cluster_game_nums = ini_get_int("Common", "cluster_game_nums");

    for (auto i = 0; i < cluster_game_nums; ++i) {
        const char* game_no = str_format("Game%d", i);
        auto ip = ini_get_string(game_no, "ip");
        auto port = ini_get_string(game_no, "port");
        if (!ip.empty() && !port.empty()) {
            INFO_LOG("connect game %s:%s\n", ip.c_str(), port.c_str());
            g_remote_mgr.connect_remote(ip, port);
        }
    }
}

extern void main_tick(const int64_t ms_pertick);

int main(int argc, char* args[]) {

    init(argc, args);

    connect_game();

    main_tick(100);

    return 0;
}
