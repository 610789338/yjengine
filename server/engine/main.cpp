#include "boost/thread.hpp"
#include "engine/log.h"
#include "engine/boost_asio.h"
#include "engine/rpc_manager.h"
#include "engine/gvalue.h"


void rpc_nihao(GValue v) {
    INFO_LOG("rpc_nihao %d\n", v.as_int16());
}

int main() {
    INFO_LOG("start\n");

    RPC_REGISTER(rpc_nihao, uint16_t());

    boost_asio_init();
    boost_asio_start();

    // main tick
    while (true) {
        rpc_imp_input_tick();
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    INFO_LOG("end\n");
    return 0;
}