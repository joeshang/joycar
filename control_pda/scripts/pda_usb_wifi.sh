# !/bin/sh

modprobe rtl8192cu

case $1 in
    start)
        ifconfig eth0 down
        ifconfig wlan0 up
        ifconfig wlan0 192.168.1.11
        iwconfig wlan0 mode ad-hoc
        iwconfig wlan0 essid JoyCar
        ;;
    
    stop)
        ifconfig wlan0 down
        ;;
esac

exit 0

