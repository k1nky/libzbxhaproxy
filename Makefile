lib_name = libzbxhaproxy.so
lib_dir = /usr/local/lib
zbx_dir = /etc/zabbix
zbx_include = /home/user/zabbix/include
gcc_com_options = -std=c11 -fPIC -Wno-int-to-pointer-cast

zbxhaproxy: hash_table.o prime.o haproxy_stat.o haproxy_servers.o zbxhaproxy.o
	gcc -shared -Wl,-soname,$(lib_name) -o $(lib_name) zbxhaproxy.o hash_table.o prime.o haproxy_stat.o haproxy_servers.o -lm

prime.o: prime.c prime.h
	gcc -c prime.c $(gcc_com_options)

hash_table.o: hash_table.c hash_table.h
	gcc -c hash_table.c $(gcc_com_options) -Wno-implicit-function-declaration

haproxy_servers.o: haproxy_servers.c haproxy_servers.h
	gcc -c haproxy_servers.c $(gcc_com_options)

haproxy_stat.o: haproxy_stat.c haproxy_stat.h
	gcc -c haproxy_stat.c $(gcc_com_options) -Wno-implicit-function-declaration

zbxhaproxy.o: zbxhaproxy.c zbxhaproxy.h
	gcc -c zbxhaproxy.c $(gcc_com_options) -Wno-implicit-function-declaration -I$(zbx_include)

install:
	/bin/bash install.sh $(lib_name) $(lib_dir) $(zbx_dir)
	
clean:
	rm -f *.o
	rm -f *.so

