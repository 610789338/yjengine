#include <stdint.h>

#include "boost_asio.h"

extern void assist_thread_start();
extern void assist_thread_exit();
extern void update_time_now_cache();
extern void rpc_handle_regist();
extern void rpc_imp_input_tick();
extern void entity_tick();
extern void entity_exit();

void engine_init() {

    rpc_handle_regist();
    boost_asio_init();
    boost_asio_start();
    assist_thread_start();
}

void engine_tick() {
    update_time_now_cache();
    rpc_imp_input_tick();
    entity_tick();
}

void engine_exit() {
    boost_asio_exit();
    assist_thread_exit();
    entity_exit();
}

