#pragma once

#include "log.h"
#include "rpc_manager.h"
#include "remote_manager.h"
#include "boost_asio.h"
#include "gvalue.h"
#include "ini.h"
#include "mailbox.h"
#include "entity.h"
#include "entity_rpc_manager.h"
#include "entity_property_manager.h"

extern void set_engine_listen_ipport(GString ip, uint16_t port);
extern void engine_init();
extern void engine_tick();
