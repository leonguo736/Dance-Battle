# Camera Instructions
## Combine & Run
gcc -std=gnu99 -Wall -pthread -o camera camera.c -lintelfpgaup -lm && ./camera

## Load drivers
~/Linux_Libraries/drivers/load_drivers

## Compile Code
gcc -std=gnu99 -Wall -pthread -o camera camera.c -lintelfpgaup -lm


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