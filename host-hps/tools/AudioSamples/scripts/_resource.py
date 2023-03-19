import os

def resourcePath(path):
    return os.path.join(os.path.dirname(__file__), "..", "resources", path)

def loadFile(path):
    return open(resourcePath(path))

def loadFileLines(path):
    return list(loadFile(path).read().splitlines())

def loadFileGrid(path):
    return [[*s] for s in loadFile(path).read().splitlines()]

def newFile(path):
    return open(resourcePath(path), 'a')