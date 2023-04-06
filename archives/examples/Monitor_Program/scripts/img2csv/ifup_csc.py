###############################################################################
# Copyright Intel Corporation
#
# Intel FPGA University Program
#
# Color space conversion methods for the Intel FPGA University Program IP Cores
#
###############################################################################

# Black/White or Greyscale Color Modes
def ifup_csc_to_1bit_bw(r, g, b):
    pixel = ((r >> 7) + (g >> 7) + (b >> 7)) >> 1
    return pixel

def ifup_csc_from_1bit_bw(pixel):
    gray = pixel * 0xFF
    return (gray << 0) | (gray << 8) | (gray << 16)

def ifup_csc_to_8bit_grayscale(r, g, b):
    pixel = (r + (g << 1) + b) >> 2
    return pixel

def ifup_csc_from_8bit_grayscale(pixel):
    gray = pixel
    return (gray << 0) | (gray << 8) | (gray << 16)


# RGB Color Modes
def ifup_csc_to_8bit_rgb(r, g, b):
    pixel = 0
    pixel += (r >> 5) << 5
    pixel += (g >> 5) << 2
    pixel += (b >> 6) << 0
    return pixel

def ifup_csc_from_8bit_rgb(pixel):
    r = ((pixel >> 5) & 0x7) << 5
    g = ((pixel >> 2) & 0x7) << 5
    b = ((pixel >> 0) & 0x3) << 6
    return (r << 0) | (g << 8) | (b << 16)

def ifup_csc_to_9bit_rgb(r, g, b):
    pixel = 0
    pixel += (r >> 5) << 6
    pixel += (g >> 5) << 3
    pixel += (b >> 5) << 0
    return pixel

def ifup_csc_from_9bit_rgb(pixel):
    r = ((pixel >> 6) & 0x7) << 5
    g = ((pixel >> 3) & 0x7) << 5
    b = ((pixel >> 0) & 0x7) << 5
    return (r << 0) | (g << 8) | (b << 16)

def ifup_csc_to_16bit_rgb(r, g, b):
    pixel = 0
    pixel += (r >> 3) << 11
    pixel += (g >> 2) <<  5
    pixel += (b >> 3) <<  0
    return pixel

def ifup_csc_from_16bit_rgb(pixel):
    r = ((pixel >> 11) & 0x1F) << 3
    g = ((pixel >>  5) & 0x3F) << 2
    b = ((pixel >>  0) & 0x1F) << 3
    return (r << 0) | (g << 8) | (b << 16)

def ifup_csc_to_24bit_rgb(r, g, b):
    pixel = 0
    pixel += r << 16
    pixel += g <<  8
    pixel += b <<  0
    return pixel

def ifup_csc_from_24bit_rgb(pixel):
    r = ((pixel >> 16) & 0xFF)
    g = ((pixel >>  8) & 0xFF)
    b = ((pixel >>  0) & 0xFF)
    return (r << 0) | (g << 8) | (b << 16)

def ifup_csc_to_30bit_rgb(r, g, b):
    pixel = 0
    pixel += r << 20
    pixel += g << 10
    pixel += b <<  0
    return pixel

def ifup_csc_from_30bit_rgb(pixel):
    r = ((pixel >> 22) & 0xFF)
    g = ((pixel >> 12) & 0xFF)
    b = ((pixel >>  2) & 0xFF)
    return (r << 0) | (g << 8) | (b << 16)

# RGBA Color Modes
def ifup_csc_to_8bit_rgba(r, g, b):
    pixel = 0
    pixel += 3 << 6
    pixel += (r >> 6) << 4
    pixel += (g >> 6) << 2
    pixel += (b >> 6) << 0
    return pixel

def ifup_csc_from_8bit_rgba(pixel):
    r = ((pixel >> 4) & 0x3) << 6
    g = ((pixel >> 2) & 0x3) << 6
    b = ((pixel >> 0) & 0x3) << 6
    return (r << 0) | (g << 8) | (b << 16)

def ifup_csc_to_12bit_rgba(r, g, b):
    pixel = 0
    pixel += 7 << 9
    pixel += (r >> 5) << 6
    pixel += (g >> 5) << 3
    pixel += (b >> 5) << 0
    return pixel

def ifup_csc_from_12bit_rgba(pixel):
    r = ((pixel >> 6) & 0x7) << 5
    g = ((pixel >> 3) & 0x7) << 5
    b = ((pixel >> 0) & 0x7) << 5
    return (r << 0) | (g << 8) | (b << 16)

def ifup_csc_to_16bit_rgba(r, g, b):
    pixel = 0
    pixel += 15 << 12
    pixel += (r >> 4) << 8
    pixel += (g >> 4) << 4
    pixel += (b >> 4) << 0
    return pixel

def ifup_csc_from_16bit_rgba(pixel):
    r = ((pixel >> 8) & 0xF) << 4
    g = ((pixel >> 4) & 0xF) << 4
    b = ((pixel >> 0) & 0xF) << 4
    return (r << 0) | (g << 8) | (b << 16)

def ifup_csc_to_32bit_rgba(r, g, b):
    pixel = 0
    pixel += 255 << 24
    pixel += r << 16
    pixel += g <<  8
    pixel += b <<  0
    return pixel

def ifup_csc_from_32bit_rgba(pixel):
    r = ((pixel >> 16) & 0xFF)
    g = ((pixel >>  8) & 0xFF)
    b = ((pixel >>  0) & 0xFF)
    return (r << 0) | (g << 8) | (b << 16)

def ifup_csc_to_40bit_rgba(r, g, b):
    pixel = 0
    pixel += 255 << 30
    pixel += r << 20
    pixel += g << 10
    pixel += b <<  0
    return pixel

def ifup_csc_from_40bit_rgba(pixel):
    r = ((pixel >> 22) & 0xFF)
    g = ((pixel >> 12) & 0xFF)
    b = ((pixel >>  2) & 0xFF)
    return (r << 0) | (g << 8) | (b << 16)

ifup_csc_gs_functions_to_rgb   = [ifup_csc_to_1bit_bw, 0, 0, 0, 0, 0, 0, ifup_csc_to_8bit_grayscale]
ifup_csc_rgb_functions_to_rgb  = [0, ifup_csc_to_8bit_rgb, ifup_csc_to_9bit_rgb, 0, ifup_csc_to_16bit_rgb, 0, 0, ifup_csc_to_24bit_rgb, 0, ifup_csc_to_30bit_rgb]
ifup_csc_rgba_functions_to_rgb = [0, ifup_csc_to_8bit_rgba, ifup_csc_to_12bit_rgba, ifup_csc_to_16bit_rgba, 0, 0, ifup_csc_to_32bit_rgba, 0, ifup_csc_to_40bit_rgba]

ifup_csc_gs_functions_from_rgb   = [ifup_csc_from_1bit_bw, 0, 0, 0, 0, 0, 0, ifup_csc_from_8bit_grayscale]
ifup_csc_rgb_functions_from_rgb  = [0, ifup_csc_from_8bit_rgb, ifup_csc_from_9bit_rgb, 0, ifup_csc_from_16bit_rgb, 0, 0, ifup_csc_from_24bit_rgb, 0, ifup_csc_from_30bit_rgb]
ifup_csc_rgba_functions_from_rgb = [0, ifup_csc_from_8bit_rgba, ifup_csc_from_12bit_rgba, ifup_csc_from_16bit_rgba, 0, 0, ifup_csc_from_32bit_rgba, 0, ifup_csc_from_40bit_rgba]

ifup_csc_functions_to_rgb = [[ifup_csc_gs_functions_to_rgb, ifup_csc_rgb_functions_to_rgb], [0, ifup_csc_rgba_functions_to_rgb]]
ifup_csc_functions_from_rgb = [[ifup_csc_gs_functions_from_rgb, ifup_csc_rgb_functions_from_rgb], [0, ifup_csc_rgba_functions_from_rgb]]

