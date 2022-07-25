//
// Created by dev on 21/7/2022.
//
#include "../../str.h"
#include "../../socket_info.h"
#include "../../lib/reg/common.h"

#include "sip_redis.h"
#include "sip_register.h"

/* Functions that will allow operations with a Cache/DB back-end */
cachedb_funcs cdbf;


int sip_register(struct sip_msg* _m, str* aor, cachedb_con *con)
{
    str body;
    str* uri;
    str key = str_init("opensips_online");
    str value = str_init("1");
    int requested_exp = 0;

    struct sip_msg* msg = _m;
    struct to_body* from_b;
    struct hdr_field* to_hdr_field;
    exp_body_t *expires;

    LM_INFO("initializing sip_register ...\n");

    if(get_body(msg, &body) < 0) {
        LM_ERR("Failed to get body of SIP message\n");
        return -1;
    }

    LM_INFO("\n........\n%s\n........\n", msg->headers->name.s);

    /* bad luck! :-( - we have to parse it */
    /* first, get some memory */
    from_b = pkg_malloc(sizeof(struct to_body));
    if (from_b == 0) {
        LM_ERR("out of pkg_memory\n");
        return -1;
    }

    /* now parse it!! */
    memset(from_b, 0, sizeof(struct to_body));
    to_hdr_field = get_header_by_static_name(msg, "To");

    parse_to(to_hdr_field->body.s, to_hdr_field->body.s + to_hdr_field->body.len, from_b);

    LM_INFO("\n........\nfrom : %s\n........\n", from_b->uri.s);

    if (con == NULL) {
        LM_ERR("failed to connect to back-end\n");
        return -1;
    }

    expires = (exp_body_t*)msg->expires->parsed;
    if (expires != NULL) {
        LM_INFO("\n........\nexpires : %s\n........\n", expires->text);
    }

    if (cdbf.set(con,&key,&value, requested_exp) < 0) {
        LM_ERR("failed to set key\n");
        return -1;
    }

    if (!uri) {
        from_b = get_to(_m);
        //uri = &from_b->uri;
    }

    LM_INFO("\n........\nto : %s\n........\n", from_b->uri.s);

    return 1;
}