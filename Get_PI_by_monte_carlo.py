# -*- coding: utf-8 -*-
## Python Version 3.4

import random
import math

if __name__ == '__main__':
    InCircle = 0
    
    radius = input('Input Radius :')
    TheNumOfPoint = input('Input the number of point :')

    radius = int(radius)
    TheNumOfPoint = int(TheNumOfPoint)

    for i in range(0, TheNumOfPoint+1):
        Px = random.uniform(-radius,radius)
        Py = random.uniform(-radius,radius)
        if (radius >= math.sqrt((Px*Px) + (Py*Py))):
            InCircle = InCircle + 1

    result = (InCircle * 4) / (TheNumOfPoint)

    print("result = "+str(result)+"\n")
