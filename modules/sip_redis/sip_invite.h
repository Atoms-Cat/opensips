//
// Created by dev on 14/8/2022.
//

#ifndef OPENSIPS_SIP_INVITE_H
#define OPENSIPS_SIP_INVITE_H

#include "../../parser/msg_parser.h"
#include "../../cachedb/cachedb.h"

/*! \brief
 * Process REGISTER request and save it's contacts
 */
int sip_invite(struct sip_msg* msg, cachedb_con *con);

#endif //OPENSIPS_SIP_INVITE_H
