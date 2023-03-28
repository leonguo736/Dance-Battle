# Converts rgb into 16-bit color

while True:
    inp = input("Enter your color (r,g,b): ").split(", ")
    if len(inp) != 3 or any(not c.isdigit() for c in inp) or any(int(c) < 0 or int(c) > 255 for c in inp):
        print("Bad input")
    else:
        r = int(float(inp[0]) / 256 * 32)
        g = int(float(inp[1]) / 256 * 64)
        b = int(float(inp[2]) / 256 * 32)
        value = b + (g << 5) + (r << 11)
        print("Values: {}, {}, {} = {}".format(r, g, b, value))
        print("Your hex value is {}".format(hex(value)))
    