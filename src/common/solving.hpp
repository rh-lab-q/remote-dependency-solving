#ifndef _SOLVING_HPP
#define _SOLVING_HPP

//SSDS
#include "repo_handler.hpp"
#include "json_handler.hpp"

//STANDARD
#include <string>
#include <iostream>

//#include <glib-2.0/glib/gerror.h>
//#include <glib-2.0/glib/gslist.h>
#include <glib.h>

//SOLVING
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/goal.h>
#include <hawkey/repo.h>
#include <hawkey/package.h>

//LIBREPO
#include <librepo/yum.h>
#include <librepo/handle.h>

//SSDS
#include "repo_handler.hpp"
#include "json_handler.hpp"

namespace ssds_solving {

  class solve{ 
    public:
          
      solve(/* repos class instance with all enabled repos handlers */); 
      ~solve();
      void fill_sack(ssds_repo::repo_metadata &metadata);
      void query(const char* request, ssds_json::json_create &answer);
      void answer(ssds_json::json_read &client_data, ssds_json::json_create &answer);
      
      //SOLVING
      HySack sack;
      
    private:
          /* repos class instance with all enabled repos handlers */
  };

}//ssds_ solving
#endif
