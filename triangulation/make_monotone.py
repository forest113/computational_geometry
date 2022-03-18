import numpy as np

vertices = [[0,0], [0.9,2], [1,1], [2,0]]
edge_list = [[0,1], [1,3], [3,2], [2,0]]

#the line is defined as one that passes through pt1, pt2
def line_intersects_edge(pt1, pt2, vert1, vert2):
    if(pt2[0] == pt1[0]):
        a = 1
        c = 0
    else:
        a = -(pt2[1] - pt1[1])/(pt2[0] - pt1[0])
        c = 1
    if( vert2[0] == vert1[0]):
        b = 1
        d = 0
    else:
        b = -(vert2[1] - vert1[1])/(vert2[0] - vert1[0])
        d = 1
    arr_list_a = [[a,c], [b,d]]
    arr_list_b = [pt2[1] - a*pt2[0], vert2[1] - b*vert2[0]]
    A = np.array(arr_list_a)
    B = np.array(arr_list_b)
    X = np.linalg.solve(A,B)
    print(X)
    is_on_edge = True
    
    if(vert1[0] < vert2[0]):
        if(not(vert1[0] <= X[0] <=vert2[0])):
            is_on_edge = False
    else:
        if(not(vert2[0] <= X[0] <=vert1[0])):
            is_on_edge = False
    if(vert1[1] < vert2[1]):
        if(not(vert1[1] <= X[1] <=vert2[1])):
            is_on_edge = False
    else:
        if(not(vert2[1] <= X[1] <=vert1[1])):
            is_on_edge = False
    if(is_on_edge):
        return X
    else:
        return []

def polygon_lies_below(edges, verts, vertex, neighbour1, neighbour2):
    upper_hits = 0
    lower_hits = 0
    for i in range(0,len(edges)):
        mid = [(verts[neighbour1][0] + verts[neighbour2][0] + verts[vertex][0])/3, (verts[neighbour1][1] + verts[neighbour2][1] + verts[vertex][1])/3] 
        issect_point = line_intersects_edge(verts[vertex], mid, verts[edges[i][0]], verts[edges[i][1]])
        print("edge:", edges[i][0], edges[i][1])
        if(issect_point):
            
            if(issect_point == vertex):
                continue
            elif(issect_point[1]>verts[vertex][1]):
                upper_hits += 1
                print("upper")
            else:
                lower_hits += 1
                print("lower")
    if(upper_hits%2 == 0):
        return True
    else:
        return False

def classify_vertex(edges, vertex, verts):
    edge1 = -1
    edge2 = -2
    for i in range(0,len(edges)):
        if(edges[i][0] == vertex or edges[i][1] == vertex):
            if(edge1 == -1):
                edge1 = i
                vert1 = edges[i][1] if edges[i][0] == vertex else edges[i][0]
            else:
                edge2 = i
                vert2 = edges[i][1] if edges[i][0] == vertex else edges[i][0]
                break

    is_highest = False
    is_lowest = False
    if(verts[vertex][1] > verts[vert1][1] and verts[vertex][1] > verts[vert2][1]):
        is_highest = True
    elif(verts[vertex][1] < verts[vert1][1] and verts[vertex][1] < verts[vert2][1]):
        is_lowest = True


    if(not(is_highest or is_lowest)):
        return 'normal'

    lies_below = polygon_lies_below(edges,verts, vertex, vert1, vert2)
    
    if(is_highest):
        if(lies_below):
            return 'start'
        else:
            return 'split'
    if(is_lowest):
        if(lies_below):
            return 'merge'
        else:
            return 'end'
        
#print(classify_vertex(edge_list, 2, vertices))
line_intersects_edge([-1,0], [3,0], [-1,-2], [4,3])
    
