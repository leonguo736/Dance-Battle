# Camera Instructions

## Load drivers
cd ~/Linux_Libraries/drivers/ && ./load_drivers && cd ~/camera/

## Mount and Copy Images
mount UUID=BAC3-B2A7 /mnt/usb && cd ~/camera/ && rm ./images/ -r && cp /mnt/usb/images ./images -r

## Main Compile & Run
gcc -std=gnu99 -Wall -pthread -o main main.c display.c camera.c -lintelfpgaup -lm && ./main

## USB Mount
mount UUID=BAC3-B2A7 /mnt/usb

## Copy Images
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

# Alex's Instructions
Instructions for running a program:

After plugging in the DE1-SoC, turn it on, launch PuTTY, and search for a serial connection with baud rate 115200.
Restart the DE1-SoC and Linux should boot in the PuTTY prompt.
Load the drivers.
Run the program.

Compiling code:

gcc -std=gnu99 -Wall -pthread -o vgatestdriver vgatestdriver.c -lintelfpgaup -lm

When compiling with song info, comment out the use of the samples first as including them takes forever. Only when the rest of the code compiles should you compile with the samples.

VIM commands:

ggdG Clear all
i Insert
"*p Paste
esc Exit insert
:x Save and exit

Converting audio:

Download a .wav file.
Open in Audacity, Ctrl+a to select all.
Tracks -> Resample to 8000.
Tools -> Sample Data Export to my AudioSamples Python program resources folder.
Limit output to first 1 mil, L-R on same line, Linear instead of dB.
Run Python program to convert to C array syntax.