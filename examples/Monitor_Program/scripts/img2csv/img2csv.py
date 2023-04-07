###############################################################################
# Copyright Intel Corporation
#
# Intel FPGA University Program
#
# This script converts image files to CSV files for use in the load file
# function in the memory window in the Intel FPGA Monitor Program software.
#
# This was run with python 3.6.0 with the Pillow python library.
#
# Arguments: 
#    The first 6 arguments are for describing the pixel buffer in the system.
#    For arguments 4-6 see RGB valid status register options in the Video IP
#    document.
# 1) display_width          - The width of the display area in the pixel buffer
# 2) display_height         - The height of the display area in the pixel buffer
# 3) buffer_address_mode    - Addressing mode of the pixel buffer
# 4) buffer_alpha           - Alpha mode of the pixel buffer
# 5) buffer_color           - Greyscale vs RGB of the pixel buffer
# 6) buffer_color_mode      - Color mode of the pixel buffer
# 7) image_filename         - Name of the image file to be converted
#
###############################################################################

import os, sys
from PIL import Image, ImageDraw
from ifup_csc import *

if (len(sys.argv) != 8):
    print ("Usage: python img2csv.py <display_width> <display_height> <buffer_mode> <buffer_alpha> <buffer_color> <buffer_color_mode> <image_filename>")
    exit()

# parse args
input_filename  = sys.argv[7]
output_filename = input_filename + ".csv"

display_width       = int(sys.argv[1])
display_height      = int(sys.argv[2])
buffer_address_mode = int(sys.argv[3])
buffer_alpha        = int(sys.argv[4])
buffer_color        = int(sys.argv[5])
buffer_color_mode   = int(sys.argv[6])

print(input_filename)
print(output_filename)
print(str(display_width))
print(str(display_height))
print(str(buffer_address_mode))
print(str(buffer_alpha))
print(str(buffer_color))
print(str(buffer_color_mode))

# Open the input image file
try:
    image = Image.open(input_filename)
except IOError as error:
    print (error)
    exit()

print(image.format, image.size, image.mode)
if (image.mode != 'RGB'):
    print ("Only RGB mode for input images is currently supported")
    exit()

# Calculate ranges
buffer_width = display_width
if (buffer_address_mode == 0):
    buffer_width = 2**(display_width - 1).bit_length()

# Parameters
width_scale_down_factor = 1
height_scale_down_factor = 1

# Open the output csv file
f = open(output_filename, "w")

for j in range(image.size[1]):
    if (j >= display_height):
        break
    for i in range(image.size[0]):
        if (i >= buffer_width):
            break
        r, g, b = image.getpixel((i*width_scale_down_factor, j*height_scale_down_factor))
        pixel = ifup_csc_functions_to_rgb[buffer_alpha][buffer_color][buffer_color_mode](r, g, b)
        f.write(hex(pixel)[2:])
        f.write(",")
    for k in range(i + 1, buffer_width):
        f.write("0,")
    f.write("\n")

print ("Output file (" + output_filename + ") written")

