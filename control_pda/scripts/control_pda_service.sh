# !/bin/sh

base=control_pda

case $1 in
    start)
        /home/joycar/$base 192.168.1.12 5555 mjpeg /dev/fb0 > /tmp/control_pda.log 2>&1 &
        ;;
    stop)
        pid=`/bin/pidof $base`
        if [ -n "$pid" ]; then
            kill -9 $pid
        fi
        ;;
esac

exit 0

