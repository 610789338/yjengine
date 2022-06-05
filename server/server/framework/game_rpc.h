#pragma once

#include "engine/gvalue.h"


#define IPPORT_STRING(ip, port) ((ip) + ":" + std::to_string(port))

class Session;

extern map<GString, Session*> g_gates;  // ip:port -> Session
extern map<GString, GString> g_gate_remotes;
