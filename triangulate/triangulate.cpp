#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Kernel/global_functions.h>
#include <CGAL/intersections.h>
#include <CGAL/Object.h>
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_2 Point_2;
typedef Kernel::Segment_2 Segment_2;
typedef Kernel::Line_2 Line_2;

/* Center of polygon can be used for this, which can be computed using cgal library fns. */
Point_2 point_within_polygon(1,0);

/* Node for edge list. */
class Edge{
   public:
   Segment_2 *edge;
   Edge *next, *prev;
};

/* Node for stack.(implemented with array) */ 
class Vertice {
   public:
   Point_2 vert;
   std::string chain;
};

/************************/
/* data structs functions */
/************************/

void edge_list_insert(Edge **start, Edge **end, Segment_2 *edge) {
   if(*start==NULL && *end==NULL){
      Edge *new_edge = new Edge();
      new_edge->edge = edge;
      new_edge->next = NULL;
      new_edge->prev = NULL;
      *start = new_edge;
      *end = new_edge;
      return;
   }
   else{
      Edge *new_edge = new Edge();
      new_edge->edge = edge;
      new_edge->next = NULL;
      new_edge->prev = *end;
      *end = new_edge;
      new_edge->prev->next = new_edge;
   }
}


void stack_push(Vertice *stack, Vertice vert, int *stack_len) {
   stack[*stack_len] = vert;
   *(stack_len) += 1;
}

Vertice stack_pop(Vertice stack[], int *stack_len) {
   if(*stack_len == 0){
      printf("dont pop from empty stack\n");
      return stack[0];
   }
   Vertice temp = stack[*stack_len-1];
   *stack_len -= 1;
   return temp;
}

/************************/
/* polygon geometry functions */
/************************/

bool edge_issect_edge(Segment_2 edge1, Segment_2 edge2, Point_2 *point_r) {
  CGAL::Object result = CGAL::intersection(edge1,edge2);
  if(CGAL::assign(*point_r, result)) {
    return true;
  } 
  return false;
}

bool point_inside_polygon(Segment_2 edges[], int num_edges, Point_2 point) {
   int hits = 0;
   Point_2 issect_point;
   Segment_2 seg(point, point_within_polygon);
   for(int i=0; i<num_edges; i++){
     if(edge_issect_edge(seg, edges[i], &issect_point)) {
        hits++;
     }
   }
   if(hits%2 != 0){
      return false;
   }
   return true;
}

bool edge_inside_polygon(Segment_2 edges[], int num_edges, Segment_2 edge){
  int hits = 0;
  Point_2 issect_point;
  for(int i=0; i<num_edges; i++){
    if(edge_issect_edge(edge, edges[i], &issect_point)) {
       hits++;
    }
  }
  Point_2 mid((edge.source()[0] + edge.target()[0])/2, (edge.source()[1] + edge.target()[1])/2);
  if(hits==2 && point_inside_polygon(edges, num_edges, mid)){
     return true;
  }
  else{
     return false;	
  }
}

/************************/
/* Triangulation */
/************************/

Edge* triangulate(Vertice vertices[], Segment_2 edges[], int num_edges, int num_verts) {
   Edge *new_edges, *new_edges_end;
   new_edges = new_edges_end = NULL;
   Vertice stack[4];
   int stack_len = 1;
   stack[0] = vertices[0];
   //stack_push(stack, vertices[0], &stack_len);
   stack_push(stack, vertices[1], &stack_len);
   
   int extra_edges = 0;
   
   for(int i=2; i<num_verts-1; i++){
      if(stack[stack_len-1].chain == vertices[i].chain) {
         Vertice last_popped = stack_pop(stack, &stack_len);
         Segment_2 *new_edge = new Segment_2(stack[stack_len-1].vert, vertices[i].vert);
         while(edge_inside_polygon(edges, num_edges, *new_edge) && stack_len>0) {
            edge_list_insert(&new_edges, &new_edges_end, new_edge);
            last_popped = stack_pop(stack, &stack_len);
            new_edge = new Segment_2(stack[stack_len-1].vert, vertices[i].vert);
         }
         stack_push(stack, last_popped, &stack_len);
      }
      else{
         while(stack_len>1){
            Vertice last_popped = stack_pop(stack, &stack_len);
            Point_2 pt1,pt2;
            pt1 = last_popped.vert;
            pt2 = vertices[i].vert;
            Segment_2 *new_edge = new Segment_2(pt1, pt2);
            edge_list_insert(&new_edges, &new_edges_end, new_edge);
            
         }
         Vertice last_popped = stack_pop(stack, &stack_len);
         stack_push(stack, vertices[i-1], &stack_len);
         stack_push(stack, vertices[i], &stack_len);
      }
   }
   if(stack_len == 0){
      return new_edges;
   }
   Vertice last_popped = stack_pop(stack, &stack_len);
   while(stack_len>1){
      Vertice last_popped = stack_pop(stack, &stack_len);
      Point_2 pt1,pt2;
      pt1 = last_popped.vert;
      pt2 = vertices[num_verts-1].vert;
      Segment_2 new_edge(pt1, pt2);
      edge_list_insert(&new_edges, &new_edges_end, &new_edge);
   }
   
   return new_edges;
}

int main(){
   /* The polygon. */
   /*       *
          *   *
        *       *
          *   *
            *     */          
   Point_2 verts[4] = {Point_2(0,0), Point_2(0.9,2), Point_2(2,0), Point_2(1,-1)};
   Segment_2 edges[4] = {Segment_2(verts[1],verts[0]), Segment_2(verts[1],verts[2]), Segment_2(verts[2],verts[3]), Segment_2(verts[0],verts[3])};
   
   int num_edges = 4, num_verts = 0;
   
   /* Prepare doubley linked edge list. */
   Edge *left_start = new Edge();
   Edge *right_start = new Edge();
   
   left_start->edge = &(edges[0]);
   left_start->next = left_start->prev = NULL;
   Edge *left_end = left_start;
   edge_list_insert(&left_start, &left_end, &edges[3]);
   
   right_start->edge = &(edges[1]);
   right_start->next = right_start->prev = NULL;
   Edge *right_end = right_start;
   edge_list_insert(&right_start, &right_end, &edges[2]);
   
   
   /*Merge chains into vertice list sorted by y coord. */
   Vertice vertices[4];
   Edge *next_right = right_start;
   Edge *next_left = left_start;
   vertices[0].vert = right_start->edge->source();
   num_verts++;
   while(next_right!=NULL || next_left!=NULL){
      if(next_right->edge->target()[1] > next_left->edge->target()[1] || next_left == NULL){
         vertices[num_verts].vert = next_right->edge->target();
         vertices[num_verts].chain = "right";
         next_right = next_right->next;
      }
      else if(next_right->edge->target()[1] < next_left->edge->target()[1] || next_right == NULL){
         vertices[num_verts].vert = next_left->edge->target();
         vertices[num_verts].chain = "left";
         next_left = next_left->next;
      }
      else{
         if(next_right->edge->target()[0] > next_left->edge->target()[0]) {
            vertices[num_verts].vert = next_right->edge->target();
            vertices[num_verts].chain = "right";
            next_right = next_right->next;
         }
         else if(next_right->edge->target()[0] < next_left->edge->target()[0]){
            vertices[num_verts].vert = next_left->edge->target();
            vertices[num_verts].chain = "left";
            next_left = next_left->next;
         }
         else{
            vertices[num_verts].vert = next_left->edge->target();
            vertices[num_verts].chain = "end";
            next_left = next_left->next;
            next_right = next_right->next;
         }
      }
      num_verts++;
   }
   
   
   Edge *next = triangulate(vertices, edges, num_edges, num_verts);
   std::cout<<"new edges:\n";
   while(next != NULL){
       std::cout << "(" << next->edge->source()[0] << "," << next->edge->source()[1] << ")->";
       std::cout << "(" << next->edge->target()[0] << "," << next->edge->target()[1] << ")\n";
       next = next->next;
   }
   
   return 0;
   
}
