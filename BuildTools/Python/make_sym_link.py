import os
import sys

def main(argv):
    if not os.path.exists(argv[1]):
        if not os.path.exists(argv[0]) :
            raise ValueError(str(argv[0]) + " does not exist")
        os.symlink(argv[0], argv[1])

if __name__ == "__main__":
   main(sys.argv[1:])