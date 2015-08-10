#ifndef _GLOBVAR_H
#define _GLOBVAR_H

#ifdef __cplusplus
extern "C"{
#endif

  /**********************************************************/
  /* Message codes used in json communication on both sides */
  /**********************************************************/

  enum{
       GENERATE_ID = 1,	
       SEND_SOLV,
       SOLV_MORE_FRAGMENT,
       SOLV_NO_MORE_FRAGMENT,
       SEND_REPO,
       GET_DEPENDENCY,
       GET_INSTALL,
       GET_UPDATE,
       ANSWER_OK,
       ANSWER_WARNING,
       ANSWER_ERROR
  };


#ifdef __cplusplus
}
#endif
#endif
