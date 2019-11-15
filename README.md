# libzbxhaproxy [![Build Status](https://travis-ci.org/k1nky/libzbxhaproxy.svg?branch=master)](https://travis-ci.org/k1nky/libzbxhaproxy) [![BCH compliance](https://bettercodehub.com/edge/badge/k1nky/libzbxhaproxy?branch=master)](https://bettercodehub.com/)
This project provides monitoring of HAProxy servers. **libzbxhaproxy** is natively compiled Zabbix agent module, written in C. The module enables discovery and present HAProxy servers stats and general information.

RPM packages for RHEL 7 are available in the [Release](https://github.com/k1nky/libzbxhaproxy/releases/) .


## How it works
**libzbxhaproxy** is  a shared library used by Zabbix agent daemon and loaded on startup. The module extends Zabbix agent functionality with keys:

- `haproxy.discovery[<socket>]` - discovering all  HAProxy servers (FRONTEND, BACKEND)

- `haproxy.info[<socket>,<pxname>,<svname>,<key>]` - present general information of HAProxy instance, such as Version, Uptime, etc

- `haproxy.stat[<socket>,<pxname>,<svname>,<key>]` - present stats of HAProxy servers

More about Zabbix agent loadable modules see [Zabbix documentation](https://www.zabbix.com/documentation/4.0/manual/config/items/loadablemodules).

## Installation
1. You will need Zabbix headers for the assembly of the module. Download and unpack Zabbix source from [official site](https://www.zabbix.com/download_sources). 

2. Clone **libzbxhaproxy** repository.

	`$ git clone https://github.com/k1nky/libzbxhaproxy.git`

3. In *"libzbxhaproxy"* directory: 

	`$ make zbx_include=<ZABBIX_INCLUDE>`
> ZABBIX_INCLUDE is path to directory *"include"* from Zabbix source archive (see step i)

4. `$ sudo make install`
 
5. Now you have **libzbxhaproxy**, default placed in *"/usr/local/lib"*. Then you should modify Zabbix agent configuration (*"zabbix_agentd.conf"*) and set directive 

	`LoadModule=/usr/local/lib/libzbxhaproxy.so`
	
6. Please restart Zabbix agent. You can verify that the module has started by Zabbix agent log:

	`XXXX:XXXXXXXX:XXXXX.XXX using configuration file: /etc/zabbix/zabbix_agentd.conf`
	`XXXX:XXXXXXXX:XXXXX.XXX starting agent module zbxhaproxy`
	`XXXX:XXXXXXXX:XXXXX.XXX loaded modules: libzbxhaproxy.so`
	`XXXX:XXXXXXXX:XXXXX.XXX agent #0 started [main process]`


## Post-Installation
Currently metrics values is obtained via HAProxy stats UNIX socket. Please, make sure stats option enabled in your HAProxy server configuration like:

`stats socket /var/lib/haproxy/stats level user`

Also, Zabbix agent daemon user must have read/write access to the stats socket file. For example, one of the solutions:

1. Get uid Zabbix user: 

	`$ id zabbix` 
	`> uid=123(zabbix) gid=128(zabbix) groups=128(zabbix) ...`

2. Modify HAProxy server configuration: 

	`stats socket /var/lib/haproxy/stats level user uid <zabbix_uid> mode 660` 

More information about HAProxy stats you can find in [HAProxy Documentation](https://cbonte.github.io/haproxy-dconv/1.8/management.html#9.3).


## Zabbix templates
A preconfigured Zabbix Template is also included in the project. You can take it from templates directory.

- `zbx_haproxy_template.xml` - preconfigured HAProxy template contains discovery rule and general HAProxy information

Set path to the HAProxy stats socket file by template defined macros `{$HAPROXY_SOCKET}`.
	
