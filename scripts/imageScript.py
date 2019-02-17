import numpy as np
from PIL import Image
f = open("tri.txt","r")
f = f.readlines()
row = len(f)
col = len(f[0].strip())
print(row)
print(col)
# Create a 1024x1024x3 array of 8 bit unsigned integers
data = np.full( (row,col,3),[255,255,255], dtype=np.uint8 )

for i, line in enumerate(f):
    line = line.strip()
    for j, char in enumerate(line):
        print(char, end="")
        if char == '1':
            data[i,j]=[0,0,0]
    print()
img = Image.fromarray(data)      # Create a PIL image
img.save("img1.png","PNG")                      # View in default viewer
