//
// Created by dev on 21/7/2022.
//
#include "../../str.h"
#include "../../socket_info.h"
#include "../../lib/reg/common.h"
#include "../../lib/cJSON.h"

#include "sip_redis.h"
#include "sip_register.h"

#define MAX_RAW_QUERY_SIZE	512

/* Functions that will allow operations with a Cache/DB back-end */
cachedb_funcs cdbf;

static char aor[MAX_RAW_QUERY_SIZE];
static str aor_str;
static char expires[MAX_RAW_QUERY_SIZE];
static str expires_str;
static char transport[MAX_RAW_QUERY_SIZE];
static str transport_str;
static str socketJson_str;


cJSON* get_all_socket(void)
{
    cJSON *socket_json = cJSON_CreateObject(), *item = cJSON_CreateObject();
    struct socket_info *si;
    int i;

    for (i = PROTO_FIRST; i < PROTO_LAST; i++) {
        if (protos[i].id == PROTO_NONE)
            continue;

        for (si = protos[i].listeners; si; si = si->next) {
//            printf("%s:%s:%s\n", protos[i].name, si->address_str.s, si->port_no_str.s);
            cJSON_AddItemToObject(item, "ip", cJSON_CreateString(si->address_str.s));
            cJSON_AddItemToObject(item, "port", cJSON_CreateString(si->port_no_str.s));
            cJSON_AddItemToObject(item, "name", cJSON_CreateString(si->name.s));
            cJSON_AddItemToObject(item, "socket", cJSON_CreateNumber(si->socket));
            cJSON_AddItemReferenceToObject(socket_json, protos[i].name, item);
            item = cJSON_CreateObject();
        }
    }

    LM_INFO("socket_json: %s\n", cJSON_Print(socket_json));
    return socket_json;
}

int sip_register(struct sip_msg* msg, str* domain, cachedb_con *con)
{
    int i;
    str body;
    int *requested_exp = 0;
    cJSON *socket_json;
    struct to_body* toBody;
    struct hdr_field* to_hdr_field;

    LM_INFO("initializing sip_register ...\n");

    if(get_body(msg, &body) < 0) {
        LM_ERR("Failed to get body of SIP message\n");
        return -1;
    }

    /* bad luck! :-( - we have to parse it */
    /* first, get some memory */
    toBody = pkg_malloc(sizeof(struct to_body));
    if (toBody == 0) {
        LM_ERR("out of pkg_memory\n");
        return -1;
    }

    /* get sip `to` header to to_body */
    memset(toBody, 0, sizeof(struct to_body));
    to_hdr_field = get_header_by_static_name(msg, "To");
    parse_to(to_hdr_field->body.s, to_hdr_field->body.s + to_hdr_field->body.len, toBody);
    /* get sip `to` header to str */
    i = snprintf(aor, sizeof(aor), "%.*s", toBody->uri.len, ZSW(toBody->uri.s));
    aor_str.s = aor;
    aor_str.len = i;

    /* get sip `expires` header to str */
    i = snprintf(expires, sizeof(expires), "%.*s", msg->expires->body.len, ZSW(msg->expires->body.s));
    expires_str.s = expires;
    expires_str.len = i;
    str2int(&expires_str, &requested_exp);

    if (con == NULL) {
        LM_ERR("failed to connect to back-end\n");
        return -1;
    }

    // 获取opensips服务地址
    socket_json = get_all_socket();

    // register 时使用那个协议 transport
    if (msg->via1 != NULL) {
        /* get sip `transport` header via1 to str */
        i = snprintf(transport, sizeof(transport), "%.*s", msg->via1->transport.len, ZSW(msg->via1->transport.s));
        transport_str.s = transport;
        transport_str.len = i;
        cJSON_AddItemToObject(socket_json, "transport", cJSON_CreateString(transport_str.s));
    }

    socketJson_str.s = cJSON_Print(socket_json);
    socketJson_str.len = strlen(cJSON_Print(socket_json));

    if (cdbf.set(con, &aor_str, &socketJson_str, requested_exp) < 0) {
        LM_ERR("failed to set key\n");
        return -1;
    }

    return 1;
}