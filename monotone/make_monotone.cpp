#include <iostream>
#include <stdio.h>
#include <string>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/intersections.h>
#include <CGAL/Object.h>
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_2 Point_2;
typedef Kernel::Segment_2 Segment_2;
typedef Kernel::Line_2 Line_2;

enum classification { normal, start, split, merge, end };

/* Node for edge list. */
class Edge{
   public:
   Segment_2 *edge;
   Edge *next;
};

void edge_list_insert(Edge **start, Segment_2 *edge) {
   if(*start==NULL){
      Edge *new_edge = new Edge();
      new_edge->edge = edge;
      new_edge->next = NULL;
      *start = new_edge;
      return;
   }
   else{
      Edge *end = *start;
      while(end->next !=NULL){
         end = end->next;
      }
      Edge *new_edge = new Edge();
      new_edge->edge = edge;
      new_edge->next = NULL;
      end->next = new_edge;
   }
}


/************************/
/* Intersection functions */
/************************/
bool line_issect_edge(Line_2 line, Segment_2 seg, Point_2 *point_r) {
  CGAL::Object result = CGAL::intersection(seg,line);
  if(CGAL::assign(*point_r, result)) {
    return true;
  } 
  return false;
}

bool polygon_lies_on_right_vert(Segment_2 edges[], int num_edges, Point_2 verts[], int vertex){
  int hits = 0;
  Point_2 pt = Point_2(0, verts[vertex][1]);
  Line_2 line(verts[vertex], pt);
  Point_2 issect_point;
  
  for(int i=0; i<num_edges; i++){
    if(line_issect_edge(line, edges[i], &issect_point)) {
      if(issect_point[0]>verts[vertex][0]){
        hits++;
      }
    }
  }
  if(hits%2 == 1){
    return true;
  }
  else{
    return false;
  }
}

bool polygon_lies_below_vert(Segment_2 edges[], int num_edges, Point_2 verts[], int vertex, Point_2 neighbour1, Point_2 neighbour2){
  int upper_hits = 0, lower_hits = 0;
  Point_2 mid_point = Point_2((neighbour1[0] + neighbour2[0])/2, (neighbour1[1] + neighbour2[1])/2);
  Line_2 line(verts[vertex], mid_point);
  Point_2 issect_point;
  
  for(int i=0; i<num_edges; i++){
    if(line_issect_edge(line, edges[i], &issect_point)) {
      if(issect_point[1]>verts[vertex][1]){
        upper_hits++;
      }
      else if(issect_point[1]<verts[vertex][1]){
        lower_hits++;
      }
    }
  }
  if(upper_hits%2 == 0){
        return true;
  }
    else{
        return false;	
  }
}
/************************/

/* Classify vertices. */
int classify_vertex(Segment_2 edges[], int num_edges, Point_2 verts[], int vertex){
  int edge1 = -1,edge2 = -1; 	
  Point_2 vert1, vert2;
  classification c;
  for(int i=0; i<num_edges; i++){
    if(edges[i].source() == verts[vertex] || edges[i].target() == verts[vertex]){
      if(edge1 == -1){
        edge1 = i;
        vert1 = (edges[i].source() == verts[vertex])? edges[i].target() : edges[i].source();
      }
      else {
        edge2 = i;
        vert2 = (edges[i].source() == verts[vertex])? edges[i].target() : edges[i].source();
      }
    }
  }
    
    bool is_highest = false;
    bool is_lowest = false;
    if(verts[vertex][1] > vert1[1] && verts[vertex][1] > vert2[1]){
      is_highest = true;
    }
    else if(verts[vertex][1] < vert1[1] && verts[vertex][1] < vert2[1]){
      is_lowest = true;
    }
    
    if(!(is_highest or is_lowest)){
     // std::cout << "normal\n";
      c = normal;
      return c;
    }
    bool lies_below = polygon_lies_below_vert(edges, num_edges, verts, vertex, vert1, vert2);
    
    if(is_highest){
      if(lies_below){
       // std::cout << "start\n";
        c = start;
        return c;
      }
      else{
       // std::cout << "split\n";
        c = split;
        return c;
      }
    }
    if(is_lowest){
      if(lies_below){
       // std::cout << "merge\n";
        c = merge;
        return c;
      }
      else{
       // std::cout << "end\n";
        c = end;
        return c;
      }
     }
  return c;
}

/* Find edges intersecting the sweepline. */
int sweep_line(int vert, Point_2 *verts, Segment_2 *edges, int num_edges) {
   int left_edge = -1;
   Point_2 pt = Point_2(0, verts[vert][1]);
   Line_2 line(verts[vert], pt);
   Point_2 issect_point, issect_point_prev;
   
   for(int i=0; i<num_edges; i++){
      if(line_issect_edge(line, edges[i], &issect_point)) {
 
         if(issect_point[0]<verts[vert][0] && (left_edge == -1 || issect_point[0] > issect_point_prev[0])){
           left_edge = i;
           issect_point_prev = issect_point;
         }
      }
   }
   return left_edge;
}

/* Add extra diagonals to convert into monotone pieces. */
Edge* make_monotone(int priorityQ[], Point_2 *verts, int num_verts, Segment_2 *edges, int num_edges) {

   Edge *new_edges = NULL;

   /* helper of edge i is stored in helper[i]. */
   int *helper = (int*)malloc(num_verts*sizeof(int));
   int *types =  (int*)malloc(num_verts*sizeof(int));
   
   for(int i=0; i<num_verts; i++) {
      int v = priorityQ[i];
      int e = sweep_line(v, verts, edges, num_edges);
      
      int type = classify_vertex(edges, num_edges, verts, v);
      std::cout << "itr:" << i << " type: " << type << "\n";
      types[v] = type;
      switch(type) {
         case start:
            {
               helper[v] = v;
               break;
            }
         
         case end:
            {
               if(types[v-1] == merge){
                  Segment_2 *new_edge = new Segment_2(verts[v], verts[helper[v-1]]); 
                  edge_list_insert(&new_edges, new_edge);
               }
               break;
            }
         
         case split:
            {
               Segment_2 *new_edge = new Segment_2(verts[v], verts[helper[e]]);
               edge_list_insert(&new_edges, new_edge);
               helper[e] = v;
               break;
            }
         
         case merge:
            {
               if(types[helper[v-1]] == merge) {
                  Segment_2 *new_edge = new Segment_2(verts[v], verts[helper[v-1]]); 
                  edge_list_insert(&new_edges, new_edge);
               }
               if(types[helper[e]] == merge && helper[v-1] != helper[e]) {
                  Segment_2 *new_edge = new Segment_2(verts[v], verts[helper[e]]); 
                  edge_list_insert(&new_edges, new_edge);
               }
               helper[e] = v;
               break;
            }
         
         case normal:
            {
               if(polygon_lies_on_right_vert(edges, num_edges, verts, v)){
                  if(types[helper[v-1]] == merge){
                     Segment_2 *new_edge = new Segment_2(verts[v], verts[helper[v-1]]); 
                     edge_list_insert(&new_edges, new_edge);
                  }
                  helper[v] = v;
               }
               else if(types[helper[e]] == merge) {
                  Segment_2 *new_edge = new Segment_2(verts[v], verts[helper[e]]); 
                  edge_list_insert(&new_edges, new_edge);
                  helper[e] = v;
               }      
               break;  
            }
      }
   }
   return new_edges;
}

int main()
{
  /* polygon looks roughly like this
       * 
      *  *
     *  * *
    * *   * *
   **       **
  *           * 
                             */
  /*vertices in counterclowckwise order. */
  Point_2 verts[4] = {Point_2(0.9,2), Point_2(0,0), Point_2(1,1), Point_2(2,0)};
  Segment_2 edges[4] = {Segment_2(verts[0],verts[1]), Segment_2(verts[1],verts[2]), Segment_2(verts[2],verts[3]), Segment_2(verts[3],verts[0])};
  int num_verts = 4;
  int num_edges = 4;
  
  /* Make priority Q. */
  int priorityQ[4] = {0,1,2,3};
  for(int i=0; i<num_verts-1; i++){
     for(int j=0; j<num_verts-i-1; j++){
        if(verts[j][1] < verts[priorityQ[j+1]][1] || (verts[priorityQ[j]][1] == verts[priorityQ[j+1]][1] && verts[priorityQ[j]][0] > verts[priorityQ[j+1]][0])) {
           int temp = priorityQ[j];
           priorityQ[j] = priorityQ[j+1];
           priorityQ[j+1] = temp;
        }
     }
  }
  
  Edge *new_edges = make_monotone(priorityQ, verts, num_verts, edges, num_edges);
  Edge *edge = new_edges;
  while(edge!=NULL){
     std::cout << "(" << edge->edge->source()[0] << "," << edge->edge->source()[1] << ")->";
     std::cout << "(" << edge->edge->target()[0] << "," << edge->edge->target()[1] << ")\n";
     edge = edge->next;
  }
  

  return 0;
}
