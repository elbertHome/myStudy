#!/bin/sh

if test $# -ne 2 ; then
    echo "USAGE: sh start.sh <CONFIG> <LOGIN_ID>"
	exit
fi

env LD_LIBRARY_PATH=/opt/zimbra/wk_ddc2/msgmv_for_compile/lib:/opt/zimbra/wk_ddc2/msgmv_for_compile/lib/mysql:/opt/zimbra/wk_ddc2/msgmv_for_compile/lib/openssl valgrind --tool=memcheck --leak-check=full /opt/zimbra/wk_ddc2/1_msgmv/bin/msgmv ${1} ${2} 2>>/opt/zimbra/wk_ddc2/1_msgmv/log/mv_valgrind.log

