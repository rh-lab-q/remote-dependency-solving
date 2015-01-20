#ifdef __cplusplus
extern "C"{
#endif
  
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include "hawkey_ex.h"
#include <stddef.h>
  
void example_func(){
  HySack sack = hy_sack_create(NULL, NULL, NULL,HY_MAKE_CACHE_DIR);
  
  
}

#ifdef __cplusplus
}
#endif