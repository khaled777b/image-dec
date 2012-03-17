#ifndef GRID2_H
#define GRID2_H

typedef int range[2];

typedef struct {
  int   base;
  range ran[2];
} grid2;


#define SizeRange(r) (r[1]-r[0])
#define SizeDim(g,d) (SizeRange((g).ran[d]))

#define Width(g)     SizeDim(g,0)
#define Height(g)    SizeDim(g,1)
#define Size(g)      (Width(g)*Height(g))
#define NumEdges(g)  (Width(g)*(Height(g)-1) + (Width(g)-1)*Height(g))

#define Index(g,x,y) ((g).base + (x-(g).ran[0][0])*Height(g) + (y-(g).ran[1][0]))

#define InRange(r,x) (r[0] <= x && x < r[1])
#define InGrid2(g,x,y) (InRange((g).ran[0],x) && InRange((g).ran[1],y))

#define Node(g,i,x,y)				\
    x = (g).ran[0][0] + (i-(g).base)/Height(g);	\
    y = (g).ran[1][0] + (i-(g).base)%Height(g);

#define VEIndex(g,x,y) (Index(g,x,y) - x)
#define HEIndex(g,x,y) (Index(g,x,y) + Width(g)*(Height(g)-1))

#endif
