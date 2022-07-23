//
// Created by dev on 21/7/2022.
//

#ifndef OPENSIPS_SIP_REGISTER_H
#define OPENSIPS_SIP_REGISTER_H

#include "../../parser/msg_parser.h"

/*! \brief
 * Process REGISTER request and save it's contacts
 */
int sip_register(struct sip_msg* _m, str* domain, str* _table);

#endif //OPENSIPS_SIP_REGISTER_H
