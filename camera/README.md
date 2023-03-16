# Camera Instructions

## Load drivers
cd ~/Linux_Libraries/drivers/ && ./load_drivers && cd ~/camera/

## Mount and Copy Images
mount UUID=BAC3-B2A7 /mnt/usb && cd ~/camera/ && rm ./images/ -r && cp /mnt/usb/images ./images -r

## Main Compile & Run
gcc -std=gnu99 -Wall -pthread -o main main.c display.c camera.c -lintelfpgaup -lm && ./main

## USB Mount (Kerry's ECE USB Stick)
mount UUID=BAC3-B2A7 /mnt/usb

## Copy Images from USB to ~/camera/images
cd ~/camera/ && rm ./images/ -r cp /mnt/usb/images ./images -r

## Ethernet in Lab
`xx:xx:xx:xx:xx:xx` is the Physical Address of the "Ethernet adpater Ethernet" after running `ipconfig /all` on the desktop that you stole ethernet cable from. 
ifconfig eth0 down
ifconfig eth0 hw ether C8:60:00:C9:4E:E6
ifconfig eth0 up
dhclient eth0

## Linux SD Card Setup
Replace `/etc/apt/sources.list` with "
deb http://old-releases.ubuntu.com/ubuntu/ precise main universe restricted multiverse
deb-src http://old-releases.ubuntu.com/ubuntu/ precise main universe restricted multiverse
deb http://old-releases.ubuntu.com/ubuntu/ precise-updates main universe restricted multiverse
deb-src http://old-releases.ubuntu.com/ubuntu/ precise-updates main universe restricted multiverse
deb http://old-releases.ubuntu.com/ubuntu/ precise-security main universe restricted multiverse
deb-src http://old-releases.ubuntu.com/ubuntu/ precise-security main universe restricted multiverse
"