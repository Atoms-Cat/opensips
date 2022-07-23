//
// Created by dev on 21/7/2022.
//
#include "../../str.h"
#include "../../socket_info.h"
#include "../../lib/reg/common.h"

#include "sip_redis.h"
#include "sip_register.h"

int sip_register(struct sip_msg* _m, str* domain, str* _table)
{
    str body;
    struct sip_msg* msg = _m;
    struct to_body* from_b;
    struct hdr_field* to_hdr_field;

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
    return 1;
}