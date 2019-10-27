/* 
 * File:   zbxhaproxy.h
 * Author: kinky
 *
 * Created on October 26, 2019, 11:07 PM
 */

#ifndef ZBXHAPROXY_H
#define	ZBXHAPROXY_H

#define HAPROXY_MOD_NAME "zbxhaproxy"

#include <stdint.h>
#include "haproxy_stat.h"
#include <stdarg.h>

#define HAVE_TIME_H 1
#include <sysinc.h>
#include <module.h>
#include <common.h>
#include <log.h>

static int zbxhaproxy_info(AGENT_REQUEST *request, AGENT_RESULT *result);
static int zbxhaproxy_test(AGENT_REQUEST *request, AGENT_RESULT *result);
static int zbxhaproxy_discovery(AGENT_REQUEST *request, AGENT_RESULT *result);
static int zbxhaproxy_stat(AGENT_REQUEST *request, AGENT_RESULT *result);

int zbx_module_api_version(void);
ZBX_METRIC *zbx_module_item_list(void);
void zbx_module_item_timeout(int timeout);
int zbx_module_init(void);
int zbx_module_uninit(void);

static int item_timeout = 0;

static ZBX_METRIC keys[] =
/*	KEY			FLAG		FUNCTION	TEST PARAMETERS */
{
	{"haproxy.info",	CF_HAVEPARAMS,	zbxhaproxy_info,	"/run/haproxy/admin.sock,Version"},
	{"haproxy.stat",	CF_HAVEPARAMS,	zbxhaproxy_stat,	"/run/haproxy/admin.sock,stats,BACKEND,status"},
	{"haproxy.discovery",	CF_HAVEPARAMS,	zbxhaproxy_discovery,	"/run/haproxy/admin.sock"},
        {"haproxy.test",	0,              zbxhaproxy_test,	""},
	{NULL}
};



#endif	/* ZBXHAPROXY_H */

