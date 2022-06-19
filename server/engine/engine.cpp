#include <stdint.h>

#include "boost_asio.h"
#include "rpc_manager.h"

void engine_init() {

    boost_asio_init();
    boost_asio_start();
    rpc_handle_regist();
}

void engine_tick() {
    rpc_imp_input_tick();
}

