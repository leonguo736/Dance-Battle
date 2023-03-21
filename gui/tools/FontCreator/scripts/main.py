import pygame, sys, copy, jsonpickle
from pygame.locals import *
from _resource import *
from _linalg import *

PIXELS_PER_SQUARE = 50

SQUARE_COLORS = ((255, 255, 255), (0, 0, 0))

class App:
    def __init__(self, name, w, h, data = None):
        self.name = name
        self.size = (w, h)
        self.windowSurface = pygame.display.set_mode(scaleV(self.size, PIXELS_PER_SQUARE))

        # Array of columns
        self.currentCharData = [[0 for y in range(h)] for x in range(w)]
        self.allCharData = data if data else {}

        self.loop()

    def save(self):
        deleteFile("fonts/" + self.name + "/" + self.name + ".txt")
        writeTextFile("fonts/" + self.name + "/" + self.name + ".txt", jsonpickle.encode([*self.size, self.allCharData]))

        deleteFile("fonts/" + self.name + "/" + self.name + ".h")
        f = newFile("fonts/" + self.name + "/" + self.name + ".h")
        f.write("#include <stdio.h>\n\n")
        f.write("int {}[{}][{}][{}] = {{".format(self.name, 122, *self.size))
        for c in range(123):
            if chr(c) in self.allCharData:
                f.write("{{ {} }}".format(", ".join(["{{ {} }}".format(", ".join([str(self.allCharData[chr(c)][x][y]) for y in range(self.size[1])])) for x in range(self.size[0])])) + ",")
            else:
                f.write("{{ {} }}".format(", ".join(["{{ {} }}".format(", ".join(['0' for y in range(self.size[1])])) for x in range(self.size[0])])) + ",")
        f.write(" };")

    def loop(self):
        while True:
            mousePressed = pygame.mouse.get_pressed(3)
            mousePos = pygame.mouse.get_pos()
            shift = pygame.key.get_pressed()[K_LSHIFT]

            if mousePressed[0] or mousePressed[2]:
                squarePos = intV(scaleV(mousePos, 1.0 / PIXELS_PER_SQUARE))
                if not (squarePos[0] < 0 or squarePos[1] < 0 or squarePos[0] >= self.size[0] or squarePos[1] >= self.size[1]):
                    self.currentCharData[squarePos[0]][squarePos[1]] = int(mousePressed[0] or not mousePressed[2])

            for x in range(self.size[0]):
                for y in range(self.size[1]):
                    pygame.draw.rect(self.windowSurface, SQUARE_COLORS[self.currentCharData[x][y]], scaleV((x, y), PIXELS_PER_SQUARE) + vOfConstants(PIXELS_PER_SQUARE, 2))
            
            for event in pygame.event.get():
                if event.type == KEYDOWN:
                    if event.key == K_ESCAPE:
                        self.save()
                        pygame.quit()
                        sys.exit()
                    elif event.key == K_RETURN:
                        self.save()
                    elif event.key == K_BACKSPACE:
                        self.currentCharData = [[0 for y in range(self.size[1])] for x in range(self.size[0])]
                    elif event.unicode.isalnum():
                        if shift:
                            self.currentCharData = copy.deepcopy(self.allCharData[event.unicode.lower()])
                        else:
                            self.allCharData[event.unicode] = copy.deepcopy(self.currentCharData)

            pygame.display.update()

if __name__ == '__main__':
    while True:
        print("\nType 'h' for help.")
        inp = input("\n> ")

        if inp == 'h':
            print("\nCommands:")
            print("\nc [name]: Create new font")
            print("\no [name]: Open existing font")
            print("\nd [existing] [new]: Duplicate an existing font")
        elif inp[0:2] == 'c ':
            name = inp[2:]
            w = int(input("\nWidth: "))
            h = int(input("Height: "))
            mkdir("fonts/" + name + "/")
            app = App(name, w, h)
        elif inp[0:2] == 'o ':
            name = inp[2:]
            app = App(name, *jsonpickle.decode(loadFile("fonts/" + name + "/" + name + ".txt").read()))
        elif inp[0:2] == 'd ':
            params = inp.split(" ")
            name = params[2]
            mkdir("fonts/" + name + "/")
            app = App(name, *jsonpickle.decode(loadFile("fonts/" + params[1] + "/" + params[1] + ".txt").read()))
        else:
            print("\nUnrecognized command")