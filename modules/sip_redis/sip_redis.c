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
#include "../../cachedb/cachedb.h"

#include "sip_redis.h"
#include "sip_register.h"

str cachedb_url;

/* Actual connection to the Cache/DB back-end */
cachedb_con *con;

static int  mod_init(void);
static int  child_init(int);
static void mod_destroy(void);
static int get_cachedb_url(void** param);

usrloc_api_t ul;

static cmd_export_t cmds[] = {
        {"sip_register", (cmd_function)sip_register, {
                {CMD_PARAM_STR, 0, 0},
                {CMD_PARAM_STR, get_cachedb_url, 0},
                {0,0,0}},
         REQUEST_ROUTE|ONREPLY_ROUTE}
};

static param_export_t params[]={
        { "cachedb_url",                 STR_PARAM, &cachedb_url.s},
        {0,0,0}
};

struct module_exports exports = {
        "sip_redis",
        MOD_TYPE_DEFAULT,/* class of this module */
        MODULE_VERSION,
        DEFAULT_DLFLAGS, /* dlopen flags */
        NULL,        /* load function */
        NULL,        /* OpenSIPS module dependencies */
        cmds,        /* Exported functions */
        0,           /* Exported async functions */
        params,      /* Exported parameters */
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
    cachedb_url.len = cachedb_url.s ? strlen(cachedb_url.s) : 0;
    LM_DBG("cachedb_url=%s\n", ZSW(cachedb_url.s));

    if(cachedb_url.s== NULL)
    {
        LM_ERR("URL not set!\n");
        return -1;
    }

    LM_DBG("binding to specific module, based on URL\n");
    if (cachedb_bind_mod(&cachedb_url,&cdbf) < 0) {
        LM_ERR("failed to bind to mod\n");
        return -1;
    }

    LM_DBG("initializing connection to back-end \n");
    con = cdbf.init(&cachedb_url);

    return 0;
}

static void mod_destroy(void)
{

}

static int get_cachedb_url(void **arg) {
    if (con == NULL) {
        LM_ERR("failed to connect to back-end\n");
        return -1;
    }
    *arg = con;
    return 0;
}

static int child_init(int rank)
{
    return 0;
}

