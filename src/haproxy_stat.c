#include "haproxy_stat.h"

int haproxy_init() {
    haproxy_info = ht_new();
    haproxy_socket_fd = 0;
    haproxy_stats = NULL;
    memset(haproxy_metrics, 0, sizeof(haproxy_metrics));
    stat_timestamp = time(NULL) - 2 * CACHE_TTL;
    info_timestamp = time(NULL) - 2 * CACHE_TTL;

    return HAPROXY_OK;
}

int haproxy_uninit() {
    ht_del_hash_table(haproxy_info);
    free_haproxy_servers(haproxy_stats);
    close(haproxy_socket_fd);
    return HAPROXY_OK;
}

int haproxy_connect(char* socket_path) {
    struct sockaddr_un haproxy_stat_addr;

    if ((haproxy_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        return HAPROXY_FAIL;
    }
    
    memset(&haproxy_stat_addr, 0, sizeof(struct sockaddr_un));
    haproxy_stat_addr.sun_family = AF_UNIX;
    strcpy(haproxy_stat_addr.sun_path, socket_path);
    
    if (connect(haproxy_socket_fd, (struct sockaddr*)&haproxy_stat_addr, 
            sizeof(haproxy_stat_addr)) < 0) {
        return HAPROXY_FAIL;
    }
    
    return HAPROXY_OK;
}

int haproxy_cmd(char* socket, char* cmd) {    
    
    for (int i = 0; i < MAX_RETRIES; i++) {
        if (send(haproxy_socket_fd, cmd, strlen(cmd), MSG_NOSIGNAL) > 0) {
            return HAPROXY_OK;
        }
        close(haproxy_socket_fd);
        haproxy_connect(socket);
    }
    return HAPROXY_FAIL;
}

char* haproxy_discovery(char* socket) {
    haproxy_server_t *sv = NULL;
    char *pxname, *svname;
    char *data;
    char buf[4096];
    int data_size = 4096;
    haproxy_update_stat(socket);

    sv = haproxy_stats;
    data = (char*) calloc(data_size, sizeof(char));
    if(data == NULL){
	fprintf(stderr,"Cannot allocate memory for data in haproxy_discovery\n");
	return NULL;
    }
    strcat(data, "{\"data\":[");

    while (sv != NULL) {
        pxname = sv->stat;
        svname = sv->stat + sv->offsets[1];
        sprintf(buf, "{\"{#PXNAME}\":\"%s\",\"{#SVNAME}\":\"%s\"}", pxname, svname);
	//check if we have enough of free memory to append the content of the next buffer
	if ((strlen(data) + strlen(buf) + 4) > data_size){
		//increment in one page size
		data_size = data_size + 4096;
		data = (char *) realloc(data, data_size * sizeof(char));
		if(data == NULL){
			fprintf(stderr,"Cannot realloc memory for data in haproxy_discovery\n");
		        return NULL;
		}
	}
        strcat(data, buf);
        if (sv->next != NULL) {
            strcat(data, ",");
        }
        sv = sv->next;
    }
    strcat(data, "]}");

    return data;
}

int haproxy_recv(char **ret_data) {

    int data_size = 4096, bytes_read, total_bytes_read = 0;
    char *data, buffer[4096];

    assert(*ret_data != NULL);
    memset(buffer, 0, sizeof(buffer));
    data = (char*) calloc(data_size, sizeof(char));
    if(data == NULL){
        fprintf(stderr,"Cannot allocate memory for data in haproxy_recv\n");
        return HAPROXY_FAIL;
    }

    while ((bytes_read = recv(haproxy_socket_fd, buffer, sizeof(buffer), 0)) > 0) {
    	//check if we have enough of free memory to append the content of the next buffer
        if ((total_bytes_read + bytes_read + 2) > data_size){
                //increment in one page size
                data_size = data_size + 4096;

                data = (char *) realloc(data, data_size * sizeof(char));
                if(data == NULL){
                        fprintf(stderr,"Cannot realloc memory for data in haproxy_recv\n");
                        return HAPROXY_FAIL;
                }

        }

        memcpy(data + total_bytes_read, buffer, bytes_read);
	total_bytes_read += bytes_read;

    }
    if(data != NULL){
	data[total_bytes_read]='\0';
    }
    if (bytes_read == -1){
	if(data != NULL){
	    free (data);
    	}
        return HAPROXY_FAIL;
    }

    *ret_data = data;

    return HAPROXY_OK;
}

int haproxy_update_info(char* socket) {
    char *recv_buffer;
    if (haproxy_cmd(socket, "show info\n") == HAPROXY_FAIL) goto get_info_fail;
    if (haproxy_recv(&recv_buffer) == HAPROXY_FAIL) goto get_info_fail;
    haproxy_parse_info(recv_buffer);
    if(recv_buffer != NULL){
	free(recv_buffer);
        recv_buffer = NULL;
    }
    return HAPROXY_OK;
    
get_info_fail:
    return HAPROXY_FAIL;
}

int haproxy_update_stat(char* socket) {
    char *recv_buffer;
    if (haproxy_cmd(socket, "show stat\n") == HAPROXY_FAIL) goto get_stat_fail;
    if (haproxy_recv(&recv_buffer) == HAPROXY_FAIL) goto get_stat_fail;
    haproxy_parse_stat(recv_buffer);
    if(recv_buffer != NULL){ 
        free (recv_buffer);
        recv_buffer = NULL;
    }
    return HAPROXY_OK;
    
get_stat_fail:
    return HAPROXY_FAIL;
}

void haproxy_parse_info(char* s) {
    char *line, *d;
    char key[255], value[255];
    int n = 0;

    line = (char*)strtok(s, "\n");
    
    while (line != NULL) {
        memset(key, 0, 255);
        memset(value, 0, 255);
        
        d = strchr(line, ':');
        // key length
        n = (d - line) / sizeof(char);
        memcpy(key, line, n);
        // value length
        n = strlen(line) - n -2;
        memcpy(value, d + 2 * sizeof(char), n);
        
        ht_insert(haproxy_info, key, value);
        
        line = (char*)strtok(NULL, "\n");
    }
}

void haproxy_parse_stat_line(char* stat) {
    char *d = NULL;
    haproxy_server_t* item;    
    int i = 0;
        
    item = new_haproxy_server(stat);
    item->offsets[i++] = 0;
    d = item->stat;
    while ((d = strchr(d, ',')) != NULL) {
        *d = '\0';
        item->offsets[i++] = (d - item->stat + 1) / sizeof(char);
        d++;
    }
    item->num_offsets = i;
        
    haproxy_stats = update_haproxy_servers(haproxy_stats, item);
}

void haproxy_parse_metrics(char* s) {
    char *d = s + 2;
    char *metric = d;
    int i = 0;
        
    if (haproxy_metrics[0]) return;
    while ((d = strchr(d, ',')) != NULL) {
        *d = '\0';
        haproxy_metrics[i++] = strdup(metric);
        metric = ++d;
    }   
}

void haproxy_parse_stat(char* s) {
    char *line;

    line = (char*)strtok(s, "\n");
    
    while (line != NULL) {        
        if (line[0] == '#') {
		haproxy_parse_metrics(line);
	}
        else {
		haproxy_parse_stat_line(line);
	}
        line = (char*)strtok(NULL, "\n");
    }
}

char* haproxy_request_stat(char* socket, char* pxname, char* svname, char* metric) {
    char* result;
    
    if (time(NULL) - stat_timestamp > CACHE_TTL) {
        haproxy_update_stat(socket);
        stat_timestamp = time(NULL);
    }
    
    result = haproxy_metric_value(pxname, svname, metric);
    if (result == NULL) return HAPROXY_NO_DATA;
    
    return result;
}

char* haproxy_request_info(char* socket, char* key) {
    char* result;
    
    if (time(NULL) - info_timestamp > CACHE_TTL) {
        haproxy_update_info(socket);      
        info_timestamp = time(NULL);
    }
    
    result = haproxy_info_value(key);
    if (result == NULL) return HAPROXY_NO_DATA;
    
    return result;
}

char* haproxy_info_value(char* key) {
    return ht_search(haproxy_info, key);
}

char* haproxy_metric_value(char* pxname, char* svname, char* metric) {
    int metric_id;
    haproxy_server_t* item;
    
    for (metric_id = 0; metric_id < MAX_NUM_METRICS; metric_id++) {
        if (strcmp(metric, haproxy_metrics[metric_id]) == 0) break;
    }
    if (metric_id == MAX_NUM_METRICS) return NULL;
    
    if (NULL == (item = get_haproxy_server(haproxy_stats, pxname, svname))) {
        return NULL;
    }
    
    return item->stat + item->offsets[metric_id];
}
