/* 
 * File:   haproxy_servers.h
 * Author: kinky
 *
 * Created on October 26, 2019, 4:29 PM
 */

#ifndef HAPROXY_STAT_LIST_H
#define	HAPROXY_STAT_LIST_H

#include <string.h>
#include <stdlib.h>

// max number of haproxy metrics
#define MAX_NUM_METRICS 100
// max length of haproxy server statistics line
#define MAX_SIZE_STAT_LINE 8192

typedef struct haproxy_server_c {
    // full stat line (character ',' replaced by '\0')
    char stat[MAX_SIZE_STAT_LINE];
    // metric value offset in the stat line (0 - pxname, 1 - svname ...))
    // get metric value by (stat + offsets[metric_index]])
    // offset[0] is 0 always
    unsigned int offsets[MAX_NUM_METRICS];
    // number of metrics in stat line
    unsigned int num_offsets;
    // next haproxy server pointer
    struct haproxy_server_c* next;
} haproxy_server_t;

// haproxy servers stored as simple one-direction list
typedef haproxy_server_t* haproxy_servers_t;

int check_haproxy_server_name(haproxy_server_t* server, char* pxname, char* svname);
haproxy_server_t* get_haproxy_server(haproxy_servers_t servers, char* pxname, char* svname);
haproxy_server_t* get_prev_haproxy_server(haproxy_servers_t servers, char* pxname, char* svname);
haproxy_server_t* new_haproxy_server(char* stat);
haproxy_servers_t update_haproxy_servers(haproxy_servers_t servers, haproxy_server_t* server);

#endif	/* HAPROXY_STAT_LIST_H */

