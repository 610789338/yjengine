#include <stdint.h>

#include "boost_asio.h"

extern void rpc_handle_regist();
extern void rpc_imp_input_tick();
extern void timer_tick();
extern void entity_tick();

void engine_init() {

    boost_asio_init();
    boost_asio_start();
    rpc_handle_regist();
}

void engine_tick() {
    // update_time_now_cache();
    rpc_imp_input_tick();
    entity_tick();
    timer_tick();  // 处理当前帧创建的立即触发的timer
}

