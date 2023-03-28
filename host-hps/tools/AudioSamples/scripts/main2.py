from _resource import *

name = input("Name of .txt file: ")
limit = int(input("Number of samples: "))
samples = [(l.split()[0], l.split()[1]) for l in loadFileLines("raw/" + name + ".txt")[:limit]]

print("Finished gathering samples")

result = newFile("results/" + name + ".txt")
for s in samples:
    result.write(s[0] + "\n" + s[1] + "\n")

print("Finished writing")