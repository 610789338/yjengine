#include <string>

#include "boost/thread.hpp"

#include "engine/engine.h"

#include "gameplay/entities/account.h"

using namespace std;

extern void set_engine_listen_ipport(GString ip, uint16_t port);
extern void engine_init();
extern void engine_tick();
extern void create_client_instance();

GString ini_file("client.ini");

void init(int argc, char* args[]) {

    int idx = 0;
    while (idx < argc) {
        if (strcmp(args[idx], "-c") == 0) {
            ++idx;
            ini_file = args[idx];
        }

        ++idx;
    }

    create_client_instance();
}

void connect_gate() {

    auto cluster_gate_nums = ini_get_int("Common", "cluster_gate_nums");

    for (auto i = 0; i < cluster_gate_nums; ++i) {
        const char* gate_no = str_format("Gate%d", i);
        auto ip = ini_get_string(gate_no, "ip");
        auto port = ini_get_string(gate_no, "port");
        if (!ip.empty() && !port.empty()) {
            INFO_LOG("connect gate %s:%s\n", ip.c_str(), port.c_str());
            g_remote_mgr.connect_remote(ip, port);
        }
    }
}

extern void main_tick(const int64_t ms_pertick);

int main(int argc, char* args[]) {

    init(argc, args);

    connect_gate();
    //connect_gate();
    //connect_gate();
    //connect_gate();
    //connect_gate();

    // linux下boost asio先run再connect会阻塞...
    engine_init();

    INFO_LOG("main tick start\n");
    main_tick(100);
    INFO_LOG("end\n");

    return 0;
}
