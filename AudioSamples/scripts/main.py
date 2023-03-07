from _resource import *

name = input("Name of .txt file to convert: ")
limit = int(input("Number of samples: "))
samples = [(l.split()[0], l.split()[1]) for l in loadFileLines("raw/" + name + ".txt")[:limit]]

print("Finished gathering samples")

result = newFile("results/" + name + ".h")
result.write("const double " + name + "_L[1000000] = {" + ", ".join([l[0] for l in samples]) + "};")
result.write("const double " + name + "_R[1000000] = {" + ", ".join([l[1] for l in samples]) + "};")

print("Finished writing")