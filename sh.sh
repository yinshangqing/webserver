#########################################################################
# File Name: sh.sh
# Author: yinsq
# mail: 841668821@qq.com
# Created Time: 2018年01月04日 星期四 16时19分01秒
#########################################################################
#!/bin/bash

echo "waiting..."
g++ webserver.cpp -o webserver -lpthread -w
echo "complete..."
sleep 1
echo "start running..."
./webserver



