//
// Created by dev on 21/7/2022.
//
#include "../../str.h"
#include "../../socket_info.h"
#include "../../lib/reg/common.h"

#include "sip_redis.h"
#include "sip_register.h"

#define MAX_RAW_QUERY_SIZE	512

/* Functions that will allow operations with a Cache/DB back-end */
cachedb_funcs cdbf;

static char aor[MAX_RAW_QUERY_SIZE];
static str aor_str;
static char expires[MAX_RAW_QUERY_SIZE];
static str expires_str;


int sip_register(struct sip_msg* msg, str* domain, cachedb_con *con)
{
    int i;
    str body;
    int *requested_exp = 0;
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

    /* now parse it!! */
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

    LM_INFO("\n........\nexpires : %.*s\n........\n", msg->expires->body.len, ZSW(msg->expires->body.s));


    if (con == NULL) {
        LM_ERR("failed to connect to back-end\n");
        return -1;
    }

    if (cdbf.set(con, &aor_str, &aor_str, requested_exp) < 0) {
        LM_ERR("failed to set key\n");
        return -1;
    }

    return 1;
}