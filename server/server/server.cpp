#include <string>

#include "boost/thread.hpp"
#include "engine/log.h"
#include "engine/boost_asio.h"
#include "engine/rpc_manager.h"
#include "engine/gvalue.h"
#include "engine/ini.h"

using namespace std;

extern void rpc_handle_register();

void init(int argc, char* args[]) {

    const char* ini_file = "server.ini";
    g_ini.parser_ini(ini_file);

    auto port = g_ini.get_int("Listen", "port");

    INFO_LOG("init port - %d\n", port);
    
    boost_asio_init(port);
    boost_asio_start();
}

int main(int argc, char* args[]) {

    init(argc, args);

    rpc_handle_register();

    INFO_LOG("main tick start\n");

    // main tick
    while (true) {
        rpc_imp_input_tick();
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    INFO_LOG("end\n");
    return 0;
}