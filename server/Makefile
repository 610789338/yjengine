
CPP_STD = c++17

LIB_DIR = linux/lib
EXE_DIR = linux/bin

ENGINE_DIR = engine
ENGINE_OUT_DIR = linux/engine

COMMON_DIR = common
COMMON_OUT_DIR = linux/engine

BOOST_INCLUDE = /home/youjun/boost_1_78_0
BOOST_LIB = /home/youjun/boost_1_78_0/stage/lib
STD_LIB = -lpthread -lboost_thread

GAME_DIR = game
GAME_OUT_DIR = linux/game

GATE_DIR = gate
GATE_OUT_DIR = linux/gate

CLIENT_DIR = client
CLIENT_OUT_DIR = linux/client

# engine
boost_asio.o: $(ENGINE_DIR)/boost_asio.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/boost_asio.cpp -o $(ENGINE_OUT_DIR)/boost_asio.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

decode.o: $(ENGINE_DIR)/decode.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/decode.cpp -o $(ENGINE_OUT_DIR)/decode.o -std=$(CPP_STD)

encode.o: $(ENGINE_DIR)/encode.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/encode.cpp -o $(ENGINE_OUT_DIR)/encode.o -std=$(CPP_STD)

engine.o: $(ENGINE_DIR)/engine.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/engine.cpp -o $(ENGINE_OUT_DIR)/engine.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

entity.o: $(ENGINE_DIR)/entity.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/entity.cpp -o $(ENGINE_OUT_DIR)/entity.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

entity_component_manager.o: $(ENGINE_DIR)/entity_component_manager.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/entity_component_manager.cpp -o $(ENGINE_OUT_DIR)/entity_component_manager.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

entity_property_manager.o: $(ENGINE_DIR)/entity_property_manager.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/entity_property_manager.cpp -o $(ENGINE_OUT_DIR)/entity_property_manager.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

entity_property_tree.o: $(ENGINE_DIR)/entity_property_tree.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/entity_property_tree.cpp -o $(ENGINE_OUT_DIR)/entity_property_tree.o -std=$(CPP_STD)

entity_rpc_manager.o: $(ENGINE_DIR)/entity_rpc_manager.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/entity_rpc_manager.cpp -o $(ENGINE_OUT_DIR)/entity_rpc_manager.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

gvalue.o: $(ENGINE_DIR)/gvalue.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/gvalue.cpp -o $(ENGINE_OUT_DIR)/gvalue.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

ini.o: $(ENGINE_DIR)/ini.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/ini.cpp -o $(ENGINE_OUT_DIR)/ini.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

mailbox.o: $(ENGINE_DIR)/mailbox.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/mailbox.cpp -o $(ENGINE_OUT_DIR)/mailbox.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

remote_manager.o: $(ENGINE_DIR)/remote_manager.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/remote_manager.cpp -o $(ENGINE_OUT_DIR)/remote_manager.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

rpc_manager.o: $(ENGINE_DIR)/rpc_manager.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/rpc_manager.cpp -o $(ENGINE_OUT_DIR)/rpc_manager.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

timer.o: $(ENGINE_DIR)/timer.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/timer.cpp -o $(ENGINE_OUT_DIR)/timer.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

event.o: $(ENGINE_DIR)/event.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/event.cpp -o $(ENGINE_OUT_DIR)/event.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

log.o: $(ENGINE_DIR)/log.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/log.cpp -o $(ENGINE_OUT_DIR)/log.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

utils.o: $(ENGINE_DIR)/utils.cpp
	g++ -fPIC -g -c $(ENGINE_DIR)/utils.cpp -o $(ENGINE_OUT_DIR)/utils.o -I $(BOOST_INCLUDE) -std=$(CPP_STD)

engine: boost_asio.o decode.o encode.o engine.o entity.o entity_component_manager.o entity_property_manager.o entity_property_tree.o entity_rpc_manager.o gvalue.o ini.o mailbox.o remote_manager.o rpc_manager.o timer.o event.o log.o utils.o
	# g++ -fPIC -shared $(ENGINE_OUT_DIR)/*.o -o $(LIB_DIR)/libengine.so
	ar crv $(LIB_DIR)/libengine.a $(ENGINE_OUT_DIR)/*.o

clean_engine:
	rm $(ENGINE_OUT_DIR)/*


# common
tmp_utils.o: $(COMMON_DIR)/utils/utils.cpp
	g++ -fPIC -g -c $(COMMON_DIR)/utils/utils.cpp -o $(COMMON_OUT_DIR)/tmp_utils.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

common: tmp_utils.o
	ar crv $(LIB_DIR)/libcommon.a $(COMMON_OUT_DIR)/*.o

clean_common:
	rm $(COMMON_OUT_DIR)/*


# game
game_rpc.o: $(GAME_DIR)/framework/game_rpc.cpp
	g++ -fPIC -g -c $(GAME_DIR)/framework/game_rpc.cpp -o $(GAME_OUT_DIR)/game_rpc.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

migrate_rpc_game.o: $(GAME_DIR)/framework/migrate_rpc.cpp
	g++ -fPIC -g -c $(GAME_DIR)/framework/migrate_rpc.cpp -o $(GAME_OUT_DIR)/migrate_rpc_game.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

game_instance.o: $(GAME_DIR)/framework/game_instance.cpp
	g++ -fPIC -g -c $(GAME_DIR)/framework/game_instance.cpp -o $(GAME_OUT_DIR)/game_instance.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

stub_base.o: $(GAME_DIR)/framework/stub_base.cpp
	g++ -fPIC -g -c $(GAME_DIR)/framework/stub_base.cpp -o $(GAME_OUT_DIR)/stub_base.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

distributed_notify.o: $(GAME_DIR)/framework/distributed_notify.cpp
	g++ -fPIC -g -c $(GAME_DIR)/framework/distributed_notify.cpp -o $(GAME_OUT_DIR)/distributed_notify.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

aoi_component.o: $(GAME_DIR)/gameplay/components/aoi_component.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/components/aoi_component.cpp -o $(GAME_OUT_DIR)/aoi_component.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

avatar_prop_test_component.o: $(GAME_DIR)/gameplay/components/avatar_prop_test_component.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/components/avatar_prop_test_component.cpp -o $(GAME_OUT_DIR)/avatar_prop_test_component.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

create_avatar_component.o: $(GAME_DIR)/gameplay/components/create_avatar_component.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/components/create_avatar_component.cpp -o $(GAME_OUT_DIR)/create_avatar_component.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

dungeon_component.o: $(GAME_DIR)/gameplay/components/dungeon_component.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/components/dungeon_component.cpp -o $(GAME_OUT_DIR)/dungeon_component.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

itembag_component.o: $(GAME_DIR)/gameplay/components/itembag_component.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/components/itembag_component.cpp -o $(GAME_OUT_DIR)/itembag_component.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

dungeon_manager.o: $(GAME_DIR)/gameplay/entities/stubs/dungeon_manager.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/entities/stubs/dungeon_manager.cpp -o $(GAME_OUT_DIR)/dungeon_manager.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

account.o: $(GAME_DIR)/gameplay/entities/account.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/entities/account.cpp -o $(GAME_OUT_DIR)/account.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

account_prop_test.o: $(GAME_DIR)/gameplay/entities/account_prop_test.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/entities/account_prop_test.cpp -o $(GAME_OUT_DIR)/account_prop_test.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

avatar.o: $(GAME_DIR)/gameplay/entities/avatar.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/entities/avatar.cpp -o $(GAME_OUT_DIR)/avatar.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

monster.o: $(GAME_DIR)/gameplay/entities/monster.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/entities/monster.cpp -o $(GAME_OUT_DIR)/monster.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

dungeon.o: $(GAME_DIR)/gameplay/entities/dungeon.cpp
	g++ -fPIC -g -c $(GAME_DIR)/gameplay/entities/dungeon.cpp -o $(GAME_OUT_DIR)/dungeon.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

game_app.o: $(GAME_DIR)/game_app.cpp
	g++ -fPIC -g -c $(GAME_DIR)/game_app.cpp -o $(GAME_OUT_DIR)/game_app.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

game: game_rpc.o migrate_rpc_game.o game_instance.o stub_base.o distributed_notify.o aoi_component.o avatar_prop_test_component.o create_avatar_component.o dungeon_component.o itembag_component.o dungeon_manager.o account.o account_prop_test.o avatar.o monster.o dungeon.o game_app.o
	g++ -fPIC -g $(GAME_OUT_DIR)/*.o -L $(BOOST_LIB) -L $(LIB_DIR) -lengine -lcommon $(STD_LIB) -std=$(CPP_STD) -o $(EXE_DIR)/game

clean_game:
	rm $(GAME_OUT_DIR)/*

# gate
gate_rpc.o: $(GATE_DIR)/framework/gate_rpc.cpp
	g++ -fPIC -g -c $(GATE_DIR)/framework/gate_rpc.cpp -o $(GATE_OUT_DIR)/gate_rpc.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

migrate_rpc_gate.o: $(GATE_DIR)/framework/migrate_rpc.cpp
	g++ -fPIC -g -c $(GATE_DIR)/framework/migrate_rpc.cpp -o $(GATE_OUT_DIR)/migrate_rpc_gate.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

gate_instance.o: $(GATE_DIR)/framework/gate_instance.cpp
	g++ -fPIC -g -c $(GATE_DIR)/framework/gate_instance.cpp -o $(GATE_OUT_DIR)/gate_instance.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

gate_app.o: $(GATE_DIR)/gate_app.cpp
	g++ -fPIC -g -c $(GATE_DIR)/gate_app.cpp -o $(GATE_OUT_DIR)/gate_app.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

gate: gate_rpc.o migrate_rpc_gate.o gate_instance.o gate_app.o
	g++ -fPIC -g $(GATE_OUT_DIR)/*.o -L $(BOOST_LIB) -L $(LIB_DIR) -lengine $(STD_LIB) -std=$(CPP_STD) -o $(EXE_DIR)/gate

clean_gate:
	rm $(GATE_OUT_DIR)/*

# client
client_rpc.o: $(CLIENT_DIR)/framework/client_rpc.cpp
	g++ -fPIC -g -c $(CLIENT_DIR)/framework/client_rpc.cpp -o $(CLIENT_OUT_DIR)/client_rpc.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

client_instance.o: $(CLIENT_DIR)/framework/client_instance.cpp
	g++ -fPIC -g -c $(CLIENT_DIR)/framework/client_instance.cpp -o $(CLIENT_OUT_DIR)/client_instance.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

aoi_component_client.o: $(CLIENT_DIR)/gameplay/components/aoi_component_client.cpp
	g++ -fPIC -g -c $(CLIENT_DIR)/gameplay/components/aoi_component_client.cpp -o $(CLIENT_OUT_DIR)/aoi_component_client.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

avatar_prop_test_component_client.o: $(CLIENT_DIR)/gameplay/components/avatar_prop_test_component_client.cpp
	g++ -fPIC -g -c $(CLIENT_DIR)/gameplay/components/avatar_prop_test_component_client.cpp -o $(CLIENT_OUT_DIR)/avatar_prop_test_component_client.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

itembag_component_client.o: $(CLIENT_DIR)/gameplay/components/itembag_component_client.cpp
	g++ -fPIC -g -c $(CLIENT_DIR)/gameplay/components/itembag_component_client.cpp -o $(CLIENT_OUT_DIR)/itembag_component_client.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

client_account.o: $(CLIENT_DIR)/gameplay/entities/account.cpp
	g++ -fPIC -g -c $(CLIENT_DIR)/gameplay/entities/account.cpp -o $(CLIENT_OUT_DIR)/client_account.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

client_avatar.o: $(CLIENT_DIR)/gameplay/entities/avatar.cpp
	g++ -fPIC -g -c $(CLIENT_DIR)/gameplay/entities/avatar.cpp -o $(CLIENT_OUT_DIR)/client_avatar.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

client_app.o: $(CLIENT_DIR)/client_app.cpp
	g++ -fPIC -g -c $(CLIENT_DIR)/client_app.cpp -o $(CLIENT_OUT_DIR)/client_app.o -I $(BOOST_INCLUDE) -I ./ -std=$(CPP_STD)

client: client_rpc.o client_instance.o aoi_component_client.o avatar_prop_test_component_client.o itembag_component_client.o client_account.o client_avatar.o client_app.o
	g++ -fPIC -g $(CLIENT_OUT_DIR)/*.o -L $(BOOST_LIB) -L $(LIB_DIR) -lengine -lcommon $(STD_LIB) -std=$(CPP_STD) -o $(EXE_DIR)/client

clean_client:
	rm $(CLIENT_OUT_DIR)/*


all: engine common game gate client
	rm $(ENGINE_OUT_DIR)/* -rf
	rm $(COMMON_OUT_DIR)/* -rf
	rm $(GAME_OUT_DIR)/* -rf
	rm $(GATE_OUT_DIR)/* -rf
	rm $(CLIENT_OUT_DIR)/* -rf

clean:
	rm $(ENGINE_OUT_DIR)/* -rf
	rm $(COMMON_OUT_DIR)/* -rf
	rm $(GAME_OUT_DIR)/* -rf
	rm $(GATE_OUT_DIR)/* -rf
	rm $(CLIENT_OUT_DIR)/* -rf
	rm $(LIB_DIR)/* -rf

