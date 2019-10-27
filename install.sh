#!/bin/bash

LIB_NAME=$1
LIB_DIR=$2
ZBX_DIR=$3
ZBX_CONF_DIR=${ZBX_DIR}/zabbix_agentd.conf.d

if [ ! -d ${LIB_DIR} ]; then
	echo "Library directory ${LIB_DIR} is not exists"
	exit
fi

echo "Copy ${LIB_NAME} to ${LIB_DIR}"
cp -f ./${LIB_NAME} ${LIB_DIR}
 
if [ ! -d ${ZBX_CONF_DIR} ]; then
	mkdir -p ${ZBX_CONF_DIR}
fi

echo "LoadModule=${LIB_DIR}/${LIB_NAME}" > ${ZBX_CONF_DIR}/libzbxhaproxy.conf
echo "Please restart zabbix agent"
echo "And check necessary permissions. Example: sudo -u zabbix zabbix_agentd -t haproxy.check[<haproxy_socket>]"
