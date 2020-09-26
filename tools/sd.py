import sys
import os
import shutil

try:
    open("/mnt/d/boot.bin", "wb").close()
except:
    pass

f = open(sys.argv[1], "rb")
c = f.read()

new = open("/mnt/d/boot.bin", "wb+")
new.write(c)

f.close()
new.close()
