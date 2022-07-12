from PIL import Image
import numpy as np

lengthX, lengthY = 0, 0 #Each is four bytes in the binary file

cases = {0: [0, 0, 130],
        1: [0, 0, 207],
        2: [0, 0, 255],
        3: [0, 200, 0],
        4: [247, 247, 116],
        5: [184, 184, 70],
        6: [232, 155, 60], #Grasslands
        7: [245, 107, 27], #Savanna
        8: [69, 196, 43], #Plains
        9: [43, 0, 0], #Monsoon
        10: [0, 133, 13], #Seasonal Rainforest
        11: [0, 138, 13], #Evergreen Rainforest
        12: [117, 0, 0], #Mud
        13: [9, 61, 0], #Deciduous Forest
        14: [11, 33, 7], #Swamp
        15: [196, 130, 49], #Tall Grasslands
        16: [125, 123, 120], #Rocks
        17: [50, 84, 47], #Coniferous Forest
        18: [85, 125, 81], #Taiga
        19: [238, 230, 255], #Snowy
        20: [222, 191, 255], #Ice Sheet
        21: [255, 242, 207], #Beach
        22: [212, 197, 159], #Muddy Beach
        23: [35, 56, 31]} #Salt Swamp

earthFile = open("earth.bin", "rb")
for i in range(8): #Read lengths
    ind = earthFile.read(1)
    ind = int.from_bytes(ind, "little")

    if (i < 4):
        lengthX += ind * int(2**(i * 8))
    elif (i < 8):
        lengthY += ind * int(2**((i - 4) * 8))

data = np.zeros((lengthY, lengthX, 3), dtype = np.uint8)

for i in range(lengthX * lengthY):
    ind = earthFile.read(1)
    ind = int.from_bytes(ind, "little")
            
    r = i // lengthX
    c = i % lengthX
    data[r][c] = cases[ind];

img = Image.fromarray(data, "RGB")
img.save("EarthMap.png")
