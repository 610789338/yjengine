#include "avatar_prop_test_component_client.h"


void AvatarPropTestComponentClient::prop_sync_compare(const GBin& v) {

    Encoder encoder;
    get_owner()->serialize_client_all(encoder);
    encoder.write_end();

    auto server_len = v.size;
    auto client_len = encoder.get_offset();
    ASSERT_LOG(server_len == client_len, "server_len.%d != client_len.%d\n", server_len, client_len);
    //if (server_len != client_len) {
    //    ERROR_LOG("server_len.%d != client_len.%d\n", server_len, client_len);
    //    return;
    //}

    if (memcmp(v.buf, encoder.get_buf(), server_len) != 0) {
        byte_print(v.buf, server_len);
        byte_print(encoder.get_buf(), client_len);
        ASSERT_LOG(false, "server buf != client buf\n");
        //ERROR_LOG("server buf != client buf\n");
    }

    INFO_LOG("prop sync compare %d.%d\n", server_len, client_len);
}
