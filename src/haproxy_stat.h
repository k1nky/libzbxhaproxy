/* 
 * File:   haproxy_stat.h
 * Author: kinky
 *
 * Created on October 20, 2019, 10:40 PM
 */

#ifndef HAPROXY_STAT_H
#define	HAPROXY_STAT_H

#include <sys/un.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include "hash_table.h"
#include "haproxy_servers.h"


#define CACHE_TTL 30
#define HAPROXY_NO_DATA ""
#define HAPROXY_OK 0
#define HAPROXY_FAIL -1
#define MAX_NUM_METRICS 100
#define MAX_RETRIES 2
#define RECV_BUFFER_SIZE 8192

int haproxy_socket_fd;
char recv_buffer[RECV_BUFFER_SIZE];
time_t stat_timestamp;
time_t info_timestamp;
ht_hash_table* haproxy_info;
haproxy_servers_t haproxy_stats;
char* haproxy_metrics[MAX_NUM_METRICS];

int haproxy_init();
int haproxy_uninit();
void haproxy_parse_stat_line(char* stat);
int haproxy_cmd(char* socket, char* cmd);
char* haproxy_discovery(char* socket);
char* haproxy_metric_value(char* pxname, char* svname, char* metric);
char* haproxy_request_info(char* socket, char* key);
char* haproxy_request_stat(char* socket, char* pxname, char* svname, char* metric);
char* haproxy_info_value(char* name);
void haproxy_parse_info(char* s);
void haproxy_parse_stat(char* s);
void haproxy_parse_metrics(char* s);
int haproxy_recv(char* buffer, size_t size);
int haproxy_update_info(char* socket);
int haproxy_update_stat(char* socket);

#endif	/* HAPROXY_STAT_H */

