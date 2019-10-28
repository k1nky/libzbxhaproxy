# libzbxhaproxy
This project provides monitoring of HAProxy servers. **libzbxhaproxy** is natively compiled Zabbix agent module, written in C. The module enables discovery and monitoring of HAProxy servers stats and general information.

More about Zabbix agent modules see [Zabbix documentation](https://www.zabbix.com/documentation/4.0/manual/config/items/loadablemodules).

## How it works
**libzbxhaproxy** is  a shared library used by Zabbix agent daemon and loaded on startup. The module extends Zabbix agent functionality with keys:

- `haprorxy.discovery[<socket>]` - discovering all exists HAProxy servers

- `haprorxy.info[<socket>,<pxname>,<svname>,<key>]` - present general information of HAProxy instance, such as Version, Uptime, etc

- `haprorxy.stat[<socket>,<pxname>,<svname>,<key>]` - present stats of HAProxy servers

## Installation
1. You will need Zabbix headers for the assembly of the project. Download and unpack Zabbix source from [official site](https://www.zabbix.com/download_sources). 

2. Clone **libzbxhaproxy** repository. 
	For example, `git clone https://github.com/k1nky/libzbxhaproxy.git`

3. In the directory *"libzbxhaproxy"*: 
`make zbx_include=<ZABBIX_INCLUDE>`
> ZABBIX_INCLUDE is path to directory *"include"* from Zabbix source archive (see step i)

4. `sudo make install`
 
5. Now you have **libzbxhaproxy**, default placed in *"/usr/local/lib"*. Then you should modify Zabbix agent configuration (*"zabbix_agentd.conf"*) and set directive `LoadModule=/usr/local/lib/libzbxhaproxy.so`.
	
6. After that please restart Zabbix agent.

## Post-Installation
Currently metrics values is obtained via HAProxy stats UNIX socket. Please, make sure stats option enabled in your HAProxy server configuration like as `stats socket /var/lib/haproxy/stats level user`

Also, Zabbix agent daemon user must have read/write access to the stats socket file. For example, one of the solutions:

1. Get uid Zabbix user: `id zabbix` 

2. Modify HAProxy server configuration: `stats socket /var/lib/haproxy/stats level user uid <zabbix_uid> mode 660` 

More information you can find [HAProxy Documentation](https://cbonte.github.io/haproxy-dconv/1.8/management.html#9.3).


## Zabbix templates
A preconfigured Zabbix Template is also included in the project. You can take it from templates directory.

There is two template files:

- `zbx_haproxy_valuemaps.xml` - it's value maps and must be first imported (Administration -> General -> Value Mapping -> Import)

- `zbx_haproxy_template.xml` - preconfigured HAProxy template contains discovery rule and general HAProxy information
	