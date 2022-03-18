import matplotlib.pyplot as plt
import numpy as np
from operator import itemgetter

points = [[0.0,0.0],[1.0,1.0],[2.0,0.0],[1.0,0.2]]

def is_right_turn(pt1, pt2, pt3):

    vec1 = [pt2[0]-pt1[0], pt2[1]-pt1[1], 0.0]
    vec2 = [pt3[0]-pt1[0], pt3[1]-pt1[1], 0.0]
    if(np.cross(vec1,vec2)[2] < 0.0):
        return True
    else:
        print(np.cross(vec1,vec2))
        return False

def convex_hull(pts):
    CH_u = [[]] #upper hull
    CH_l = [[]] #lower hull
    pts.sort()  #sort by x coordinate
    CH_u.append(pts[0])
    CH_u.append(pts[1])
    CH_l.append(pts[len(pts)-1])
    CH_l.append(pts[len(pts)-2])

    del CH_u[0]
    del CH_l[0]

    #find upper hull
    for i in range(2, len(pts)):
        CH_u.append(pts[i])
        length = len(CH_u)
        print('chu ',CH_u)
        while(not(is_right_turn(CH_u[length-3], CH_u[length-2], CH_u[length-1]))):
              print(CH_u[length-2], 'len:', length)
              del CH_u[length-2]
              length = length-1
              if(length < 3):
                  break
    #find lower hull
    for i in range(len(pts)-3, -1):
        CH_l.append(pts[i])
        length = len(CH_l)
        print('boo')
        while(not(is_right_turn(CH_l[length-1], CH_l[length-2], CH_l[length-3]))and length>2):
              del CH_l[length-2]
              length = length-1
              if(length < 3):
                  break
    #delete first and last elements of ch lower
    if(length>1):
        length = len(CH_l)
        del CH_l[length-1]
        del CH_l[0]
    else:
        del CH_l

    print("hull: ")
    print(CH_u, CH_l)

print(is_right_turn([0.0, 0.0], [0.0, 1.0], [0.2, 0.5]))
convex_hull(points)
              
              
        
            
