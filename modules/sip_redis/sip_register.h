//
// Created by dev on 21/7/2022.
//

#ifndef OPENSIPS_SIP_REGISTER_H
#define OPENSIPS_SIP_REGISTER_H

#include "../../parser/msg_parser.h"

/*! \brief
 * Process REGISTER request and save it's contacts
 */
int sip_register(struct sip_msg* _m, void* _d, str* _f, str* _s, str* _owtag);

#endif //OPENSIPS_SIP_REGISTER_H
