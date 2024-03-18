#!/bin/sh
# Add your startup script
echo -n $FLAG > /home/ctf/flag
chmod 744 /home/ctf/flag
unset FLAG
# DO NOT DELETE
/etc/init.d/xinetd start;
sleep infinity;
