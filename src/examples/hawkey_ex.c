/*#include "../../../hawkey/hawkey/src/iutil.h"
#include "../../../hawkey/hawkey/src/types.h"
#include "../../../hawkey/hawkey/src/sack.h"
//#include <hawkey/sack.h>
#include "../../../hawkey/hawkey/src/sack_internal.h"*/
/*#include <stdio.h>
#include <solv/solver.h>
#include <solv/util.h>
#include <solv/pool.h>
#include <solv/dirpool.h>
*/
#ifdef __cplusplus
extern "C"{
#endif
  
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include "hawkey_ex.h"
#include <stddef.h>
// hawkey
/*#include "../../../hawkey/hawkey/src/errno_internal.h"
#include "../../../hawkey/hawkey/src/iutil.h"
#include "../../../hawkey/hawkey/src/package_internal.h"
#include "../../../hawkey/hawkey/src/packageset_internal.h"
#include "../../../hawkey/hawkey/src/query.h"
#include "../../../hawkey/hawkey/src/repo_internal.h"
#include "../../../hawkey/hawkey/src/util.h"
#include "../../../hawkey/hawkey/src/version.h"*/


  
void example_func(){
  HySack sack = hy_sack_create(NULL, NULL, NULL,HY_MAKE_CACHE_DIR);
  
  
}

#ifdef __cplusplus
}
#endif