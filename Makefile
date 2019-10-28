lib_name = libzbxhaproxy.so
lib_dir = /usr/local/lib
zbx_dir = /etc/zabbix
zbx_include = /home/user/zabbix/include
gcc_com_options = -std=c11 -fPIC -Wno-int-to-pointer-cast -Wno-int-conversion
gcc = gcc $(gcc_com_options) -c
build = ./build
src = ./src

.PHONY: clean install

build: build_dir zbxhaproxy

build_dir:
	mkdir -p $(build)

zbxhaproxy: hash_table.o prime.o haproxy_stat.o haproxy_servers.o zbxhaproxy.o
	gcc -shared -Wl,-soname,$(lib_name) -o $(build)/$(lib_name) $(addprefix $(build)/,$^) -lm

prime.o: $(addprefix $(src)/,prime.c prime.h)
	$(gcc) $< -o $(addprefix $(build)/,$@)

hash_table.o: $(addprefix $(src)/,hash_table.c hash_table.h)
	$(gcc) $< -o $(addprefix $(build)/,$@) -Wno-implicit-function-declaration

haproxy_servers.o: $(addprefix $(src)/,haproxy_servers.c haproxy_servers.h)
	$(gcc) $< -o $(addprefix $(build)/,$@)

haproxy_stat.o: $(addprefix $(src)/,haproxy_stat.c haproxy_stat.h)
	$(gcc) $< -o $(addprefix $(build)/,$@) -Wno-implicit-function-declaration

zbxhaproxy.o: $(addprefix $(src)/,zbxhaproxy.c zbxhaproxy.h)
	$(gcc) $< -o $(addprefix $(build)/,$@) -Wno-implicit-function-declaration -I$(zbx_include)

install:
	/bin/bash install.sh $(lib_name) $(lib_dir) $(zbx_dir)
	
clean:
	rm -rf $(build)

