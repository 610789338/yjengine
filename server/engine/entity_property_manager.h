#pragma once

#include <unordered_map>

#include "gvalue.h"
#include "log.h"

using namespace std;

// entity property manager
template<class EntityClassType>
class EntityPropertyManager {

public:
    EntityPropertyManager() {
        EntityClassType::property_define();
    }

    template<class T>
    void regist_property(enum PropType type, const GString& property_name, T _default) {
        
        if (propertys.find(property_name) != propertys.end()) {
            WARN_LOG("prop.%s exist\n", property_name.c_str());
            return;
        }

        propertys[property_name] = EntityProperty(type, GValue(_default));
    }

    unordered_map<GString, EntityProperty> propertys;
};

#define PROPERTY(type, property_name, _default) property_manager.regist_property(type, property_name, _default)
