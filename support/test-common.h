
#define WAIT                                                                   \
  for (volatile long int i = 0; i < (16 - FRAM_WAIT) * 2000l; i++)             \
    ;
