from PIL import Image
import argparse

def convert(path_in: str, path_out: str = None) -> None: 
    # Convert image to 24-bit RGB BMP
    img = Image.open(path_in)
    img = img.convert("RGB")

    img = img.resize((320, 240), Image.ANTIALIAS)

    if path_out is None:
        path_out = path_in[:path_in.rfind(".")] + ".bmp"
    print(path_out)
    img.save(path_out)

args = argparse.ArgumentParser()
args.add_argument("-i", "--input", required=True, help="path to input image")
args.add_argument("-o", "--output", required=False, help="path to output image")
args = vars(args.parse_args())

convert(args["input"], args["output"])