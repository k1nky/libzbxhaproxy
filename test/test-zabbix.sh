#!/bin/bash

TEST_OUTPUT=test/test.out
HAPROXY_SOCK=/run/haproxy/admin.sock

ZBX4_AGENT=/usr/sbin/zabbix_agentd
ZBX3_AGENT=/usr/sbin/zabbix3_agentd
ZBX_CONF=test/zabbix_agentd.conf
ZBX_SRC=$1
ZBX4_INCLUDE=$ZBX_SRC/zabbix4/include
ZBX3_INCLUDE=$ZBX_SRC/zabbix3/include

ZBX_HAPROXY_TESTS=("haproxy.info[$HAPROXY_SOCK,Uptime]" "haproxy.discovery[$HAPROXY_SOCK]")

function _skip() {
	echo -e "===> \e[30;46m[  Skip  ]\e[0m \t $1"
}

function _check() {
	if [ $# -lt 2 ]; then
		echo "Few arguments for _check"
		exit 1
	fi

	RC=$2

	if [ $RC -eq 0 ]; then
		echo -e "===> \e[30;42m[ Passed ]\e[0m \t $1"
	else
		echo -e "===> \e[30;41m[ Failed ]\e[0m \t $!"
		exit 1
	fi

}

echo "LoadModulePath=`pwd`/build" > test/zabbix_modules_path.conf

echo "==> Check Makefile"
test -f "Makefile"
_check "Makefile exist" $?

echo "==> Check make for zabbix 4"
make clean &> $TEST_OUTPUT
if [ -d $ZBX4_INCLUDE ]; then
	make zbx_include=$ZBX4_INCLUDE &>> $TEST_OUTPUT
	_check "Make build for zabbix agent 4" $?
	
	if [ -x $ZBX4_AGENT ]; then
		for test in ${ZBX_HAPROXY_TESTS[*]}; do
			$ZBX4_AGENT -c $ZBX_CONF -t $test &>> $TEST_OUTPUT
			_check "Test $test for zabbix agent 4" $?	
		done
	else
		_skip "Zabbix agent 4 not found by $ZBX4_AGENT" 
	fi
else
	_skip "Zabbix 4 source include not found by $ZBX4_INCLUDE"
fi

echo "==> Check make for zabbix 3"
make clean &>> $TEST_OUTPUT
if [ -d $ZBX3_INCLUDE ]; then
	make zbx_include=$ZBX3_INCLUDE &>> $TEST_OUTPUT
	_check "Make build for zabbix agent 3" $?
	
	if [ -x $ZBX3_AGENT ]; then
		for test in ${ZBX_HAPROXY_TESTS[*]}; do
			$ZBX3_AGENT -c $ZBX_CONF -t $test &>> $TEST_OUTPUT
			_check "Test $test for zabbix agent 3" $?	
		done
	else
		_skip "Zabbix agent 3 not found by $ZBX3_AGENT" 
	fi
else
	_skip "Zabbix 3 source include not found by $ZBX3_INCLUDE"
fi

make clean &>> $TEST_OUTPUT
exit 0

