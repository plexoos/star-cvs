#define SPT_PTR_NMAX 20000
#define SPT_X 40
#define SPT_Z 1200
#define PIX_NMAX SPT_X*SPT_Z
#define DEBUG 1

typedef struct SftInternalSpt{      
	long layer;
        long track;
        long wafer; 
        double x[3];
        double xc;
        double zc;
        double zc0;
        double rx;
        double rz;
        long   off;
        long   ix;
        long   iz;
        long   ii;
} SPACEPOINT;


typedef struct mypoint {
  float x; /* x of space point */
  float y; /* y of spacepoint */
} POINT;
