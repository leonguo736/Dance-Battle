from PIL import Image
import argparse

def convert(image: str, width: int, height: int) -> None:
    # Convert image to 24-bit RGB
    img = Image.open(image)
    img = img.convert("RGB")
    img = img.resize((width, height))
    img.save(image)

args = argparse.ArgumentParser()
args.add_argument("-i", "--input", required=True, help="path to input image")
args.add_argument("-o", "--output", required=True, help="path to output image")
args.add_argument("-w", "--width", type=int, default=320, help="width of output image")
args.add_argument("-h", "--height", type=int, default=240, help="height of output image")
args = vars(args.parse_args())

convert(args["input"], args["width"], args["height"])