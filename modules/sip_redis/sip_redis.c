//
// Created by dev on 21/7/2022.
//
/*! \brief
 * Module exports structure
 */
#include "sip_redis.h"

static int  mod_init(void);
static int  child_init(int);
static void mod_destroy(void);

usrloc_api_t ul;

static int domain_fixup(void** param);

static cmd_export_t cmds[] = {
        {"sip_register", (cmd_function)sip_register, {
                {CMD_PARAM_STR|CMD_PARAM_STATIC, domain_fixup, 0},
                {CMD_PARAM_STR|CMD_PARAM_OPT,0,0},
                {CMD_PARAM_STR|CMD_PARAM_OPT,0,0},
                {CMD_PARAM_STR|CMD_PARAM_OPT,0,0}, {0,0,0}},
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



/*! \brief
 * Convert char* parameter to udomain_t* pointer
 */
static int domain_fixup(void** param)
{
    udomain_t* d;
    str d_nt;

    if (pkg_nt_str_dup(&d_nt, (str*)*param) < 0)
        return E_OUT_OF_MEM;

    if (ul.register_udomain(d_nt.s, &d) < 0) {
        LM_ERR("failed to register domain\n");
        return E_UNSPEC;
    }

    pkg_free(d_nt.s);

    *param = (void*)d;
    return 0;
}
