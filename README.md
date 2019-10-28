# libzbxhaproxy
This project provides monitoring of HAProxy servers. **libzbxhaproxy** is natively compiled Zabbix agent module, written in C. The module enables discovery and monitoring of HAProxy servers stats and general information. Using the `LoadModule` directive in a zabbix agent configuration.

A preconfigured Zabbix Template is also included.

## How it works
**libzbxhaproxy** is  a shared library used by Zabbix agent daemon and loaded on startup. The module extends Zabbix agent functionality with keys:

- `haprorxy.discovery[<socket>]` - discovering all exists HAProxy servers

- `haprorxy.info[<socket>,<pxname>,<svname>,<key>]` - present general information of HAProxy instance, such as Version, Uptime, etc

- `haprorxy.stat[<socket>,<pxname>,<svname>,<key>]` - present stats of HAProxy servers

Currently metrics values is obtained via HAProxy stats unix socket.

## Installation
1. You need Zabbix headers for the assembly of the project. Download and unpack Zabbix source from [official site](https://www.zabbix.com/download_sources). 

2. Clone **libzbxhaproxy** repository. 
	For example, `wget https://github.com/k1nky/libzbxhaproxy.git`

3. In the directory *libzbxhaproxy*: 
`make zbx_include=<ZABBIX_INCLUDE>`
> ZABBIX_INCLUDE is path to directory *include* from Zabbix source archive (see step i)

4. `make install`