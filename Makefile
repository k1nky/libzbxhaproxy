zbxhaproxy: hash_table.o prime.o haproxy_stat.o haproxy_servers.o zbxhaproxy.o
	gcc -shared -Wl,-soname,libzbxhaproxy.so -o libzbxhaproxy.so zbxhaproxy.o hash_table.o prime.o haproxy_stat.o haproxy_servers.o -lm

prime.o: prime.c prime.h
	gcc -c prime.c -std=c11 -fPIC 

hash_table.o: hash_table.c hash_table.h
	gcc -c hash_table.c -std=c11 -fPIC -Wno-implicit-function-declaration

haproxy_servers.o: haproxy_servers.c haproxy_servers.h
	gcc -c haproxy_servers.c -std=c11 -fPIC

haproxy_stat.o: haproxy_stat.c haproxy_stat.h
	gcc -c haproxy_stat.c -std=c11 -fPIC -Wno-implicit-function-declaration

zbxhaproxy.o: zbxhaproxy.c zbxhaproxy.h
	gcc -c zbxhaproxy.c -std=c11 -fPIC -g -I/home/batman/include -Wno-int-to-pointer-cast -Wno-implicit-function-declaration

clean:
	rm -f *.o

