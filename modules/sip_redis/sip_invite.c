//
// Created by dev on 14/8/2022.
//
#include "../../str.h"
#include "../../socket_info.h"
#include "../../lib/reg/common.h"
#include "../../lib/cJSON.h"

#include "sip_redis.h"
#include "sip_invite.h"


int sip_invite(struct sip_msg* msg, cachedb_con *con)
{


    LM_INFO("initializing sip_invite ...\n");

    LM_INFO("sip_invite headers: \n........\n%s\n........\n", msg->headers->name.s);


    return 1;
}