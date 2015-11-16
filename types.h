typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

// Mutual exclusion lock.
typedef struct __lock_t {
  uint flag;       
} lock_t;
