#include "boost/thread.hpp"
#include "engine/log.h"
#include "engine/boost_asio.h"

int main() {
    INFO_LOG("start\n");

    boost_asio_init();
    boost_asio_start();

    // main tick
    while (true) {
        //boost_asio_tick();
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    INFO_LOG("end\n");
    return 0;
}