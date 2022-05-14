#include "boost/thread.hpp"
#include "engine/log.h"
#include "engine/boost_asio.h"
#include "engine/rpc_manager.h"
#include "engine/gvalue.h"
#include <iostream>

using namespace std;


void rpc_normal_param_test(GValue i8, GValue i16, GValue i32, GValue i64, GValue ui8, GValue ui16, GValue ui32, GValue ui64, GValue f, GValue d, GValue s) {
    cout << "i8." << int(i8.as_int8()) 
        << " i16." << i16.as_int16()
        << " i32." << i32.as_int32()
        << " i64." << i64.as_int64()
        << " ui8." << unsigned int(ui8.as_uint8())
        << " ui16." << ui16.as_uint16()
        << " ui32." << ui32.as_uint32()
        << " ui64." << ui64.as_uint64()
        << " float." << f.as_float()
        << " double." << d.as_double()
        << " string." << s.as_string()
        << endl;
}

int main() {
    INFO_LOG("start\n");

    RPC_REGISTER(rpc_normal_param_test, int8_t(), int16_t(), int32_t(), int64_t(), uint8_t(), uint16_t(), uint32_t(), uint64_t(), float(), double(), string());

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