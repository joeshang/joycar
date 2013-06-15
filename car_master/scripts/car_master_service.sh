# !/bin/sh

base=car_master

case $1 in
    start)
        /home/joycar/$base 5555 /dev/video2 mjpeg jpeg /dev/ttySAC2 &
        ;;

    stop)
        pid=`/bin/pidof $base`
        if [ -n "$pid" ]; then
            kill -9 $pid
        fi
        ;;
esac

exit 0

