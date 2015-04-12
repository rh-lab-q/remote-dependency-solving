#include "repo_handler.hpp"

namespace ssds_repo {
  /**********************************************************************
   *	Class for geting repository metadata by given url
   **********************************************************************/ 
  repo_metadata::repo_metadata()
  {
    //this->urls = new std::vector<ssds_xml::xml_node*>();
  }
  
  /*
   * Function tries to find repo metadata in local files and if it cannot be found it downloads them from the url
   */
  int repo_metadata::locate_repo_metadata_by_url(ssds_json::json_read &json)
  {
    int len = g_slist_length(json.repoInfoList);
    for(int i=0;i<len;i++){
      struct ssds_json::json_read::repoInfo* repo = (struct ssds_json::json_read::repoInfo*)g_slist_nth_data(json.repoInfoList, i);
      download_repo_metadata_by_url(repo);
    }
  }
  
  
  /*
   * 	Downloads repo metadata based on given url address
   */
  void repo_metadata::download_repo_metadata_by_url(ssds_json::json_read::repoInfo* repo_info)
  {//TODO - more than one url
    LrHandleOption type;
    GError *tmp_err = NULL;
    
    // Download only this metadata
    char *download_list[] = { "primary", "filelists", NULL};
    LrHandle *h = lr_handle_init();
    LrResult *r = lr_result_init();
    repo_info->urls[repo_info->count]=nullptr;
    //find type of url in vector
    switch(repo_info->type)
    {
      case 1:
        type=LRO_URLS;
        lr_handle_setopt(h, NULL, type, repo_info->urls);
        break;
      case 2:
        type = LRO_MIRRORLISTURL;
        lr_handle_setopt(h, NULL, type, repo_info->urls[0]);
        break;
      case 3:
        type = LRO_METALINKURL;
        lr_handle_setopt(h, NULL, type, repo_info->urls[0]);
        break;
    }

    //"https://mirrors.fedoraproject.org/metalink?repo=updates-released-f21&arch=x86_64");
    lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);
    lr_handle_setopt(h, NULL, LRO_YUMDLIST, download_list);
    lr_handle_setopt(h, NULL, LRO_CONNECTTIMEOUT, 10);
    
    gboolean ret = lr_handle_perform(h, r, &tmp_err); 	
    
    char *destdir;
    lr_handle_getinfo(h, NULL, LRI_DESTDIR, &destdir);
    
    if (ret) {
      printf("Download successfull (Destination dir: %s)\n", destdir);
      
      LrYumRepo* repo = lr_yum_repo_init();
      lr_result_getinfo(r, &tmp_err, LRR_YUM_REPO, &repo);
      
      //std::cout << lr_yum_repo_path(repo, "filelists") << std::endl;
      
      ssds_repo::metadata_files_location* loc = new ssds_repo::metadata_files_location;
      loc->repomd = destdir;
      loc->filelists = lr_yum_repo_path(repo,"filelists");
      loc->primary = lr_yum_repo_path(repo,"primary");
      
      this->files_locations.push_back(loc);

    } else {
      fprintf(stderr, "Error encountered: %s\n", tmp_err->message);
      g_error_free(tmp_err);
    }
    
    lr_result_free(r);
    lr_handle_free(h);
  }  
}