###############################################################################
# Copyright Intel Corporation
#
# Intel FPGA University Program
#
# This script converts CSV files to jpg files for use in the store memory
# contents to a file function in the memory window in the Intel FPGA Monitor 
# Program software.
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

import os, sys, csv
from PIL import Image, ImageDraw
from ifup_csc import *

if (len(sys.argv) != 8):
    print ("Usage: python csv2img.py <display_width> <display_height> <buffer_mode> <buffer_alpha> <buffer_color> <buffer_color_mode> <image_filename>")
    exit()

# parse args
input_filename  = sys.argv[7]
output_filename = input_filename + ".jpg"

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

# Open and load the input csv file
csv_file = open(input_filename, "r")
csv_data = csv.reader(csv_file, delimiter=',')

# Calculate ranges
buffer_width = display_width
if (buffer_address_mode == 0):
    buffer_width = 2**(display_width - 1).bit_length()

# Create the output image
image = Image.new("RGB", (display_width, display_height))

def copy_csv_data_to_image():
    width = 0
    height = 0
    for row in csv_data:
        for csv_pixel in row:
            if width < display_width:
                image_pixel = ifup_csc_functions_from_rgb[buffer_alpha][buffer_color][buffer_color_mode](int(csv_pixel, 16))
                image.putpixel((width, height), image_pixel)
            width += 1
            if width == buffer_width:
                width = 0
                height += 1
            if height >= display_height:
                return

copy_csv_data_to_image()
image.save(output_filename)

