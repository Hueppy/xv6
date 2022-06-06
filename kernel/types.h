typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;

typedef uint64 pde_t;

// permissions
#define P_READ    04
#define P_WRITE   02
#define P_EXECUTE 01
#define P_RW      (P_READ | P_WRITE)
#define P_RWX     (P_RW | P_EXECUTE)

#define A_USER  6
#define A_GROUP 3
#define A_OTHER 0

#define MODE(PERM, ACCESSOR) (PERM << ACCESSOR)
#define PERMISSIONS(MODE, ACCESSOR) ((MODE >> ACCESSOR) & P_RWX)
