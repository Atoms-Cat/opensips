//
// Created by dev on 21/7/2022.
//
/*! \brief
 * Module exports structure
 */
#include <stdio.h>

#include "../../sr_module.h"
#include "../../timer.h"
#include "../../dprint.h"
#include "../../error.h"
#include "../../socket_info.h"
#include "../../pvar.h"
#include "../../mod_fix.h"
#include "../../lib/reg/config.h"
#include "../../lib/reg/pn.h"
#include "../../lib/reg/common.h"

#include "../usrloc/ul_mod.h"
#include "../signaling/signaling.h"

#include "sip_redis.h"
#include "sip_register.h"

static int  mod_init(void);
static int  child_init(int);
static void mod_destroy(void);

usrloc_api_t ul;

static cmd_export_t cmds[] = {
        {"sip_register", (cmd_function)sip_register, {
                {CMD_PARAM_STR, 0, 0},
                {CMD_PARAM_STR, 0, 0},
                {0,0,0}},
         REQUEST_ROUTE|ONREPLY_ROUTE}
};

struct module_exports exports = {
        "sip_redis",
        MOD_TYPE_DEFAULT,/* class of this module */
        MODULE_VERSION,
        DEFAULT_DLFLAGS, /* dlopen flags */
        NULL,        /* load function */
        NULL,        /* OpenSIPS module dependencies */
        cmds,           /* Exported functions */
        0,           /* Exported async functions */
        0,           /* Exported parameters */
        0,           /* exported statistics */
        0,           /* exported MI functions */
        0,           /* exported pseudo-variables */
        0,           /* exported transformations */
        0,           /* extra processes */
        0,           /* module pre-initialization function */
        mod_init,    /* module initialization function */
        0,
        mod_destroy, /* destroy function */
        child_init,  /* Per-child init function */
        0            /* reload confirm function */
};

static int mod_init(void)
{
    LM_INFO("initializing...\n");

    if (load_ul_api(&ul) != 0) {
        LM_ERR("failed to bind usrloc\n");
        return -1;
    }

    return 0;
}

static void mod_destroy(void)
{

}

static int child_init(int rank)
{
    return 0;
}

