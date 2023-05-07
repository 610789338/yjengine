#include <string>
#include <unordered_set>

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

unordered_set<GString> g_gate_list;

void get_gate_list() {
    auto cluster_gate_nums = ini_get_int("Common", "cluster_gate_nums");

    for (auto i = 0; i < cluster_gate_nums; ++i) {
        const char* gate_no = str_format("Gate%d", i);
        auto ip = ini_get_string(gate_no, "ip");
        auto port = ini_get_string(gate_no, "port");
        if (!ip.empty() && !port.empty()) {
            g_gate_list.insert(str_format("%s:%s", ip.c_str(), port.c_str()));
        }
    }
}

void connect_gate() {
    if (!g_gate_list.empty()) {
        auto gate_addr = *(g_gate_list.begin());
        INFO_LOG("connect gate %s\n", gate_addr.c_str());
        auto split_pos = gate_addr.find(":");
        g_remote_mgr.connect_remote(gate_addr.substr(0, split_pos), gate_addr.substr(split_pos + 1, gate_addr.size()));
        g_gate_list.erase(gate_addr);
    }
}

extern void main_tick(const int64_t ms_pertick);

int main(int argc, char* args[]) {

    init(argc, args);

    get_gate_list();
    connect_gate();
    //connect_gate();
    //connect_gate();
    //connect_gate();
    //connect_gate();

    // linux下boost asio先run再connect会阻塞...
    engine_init();

    main_tick(100);

    return 0;
}
