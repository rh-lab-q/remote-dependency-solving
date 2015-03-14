#include "repo_handler.hpp"

namespace ssds_repo {
  /**********************************************************************
   *	Class for geting repository metadata by given url
   **********************************************************************/ 
  repo_metadata::repo_metadata()
  {
    this->urls = new std::vector<ssds_xml::xml_node*>();
  }
  
  /*
   * 	Downloads repo metadata based on given url address
   */
  void repo_metadata::by_url(ssds_xml::xml_node* repo_node, std::vector<LrResult*> &repo_info)
  {
    char * url_char = (char *)repo_node->value.c_str();
    LrHandleOption type;
    
    //find type of url in vector
    for(std::vector<ssds_xml::xml_attr*>::iterator it = repo_node->attributes.begin(); it != repo_node->attributes.end(); it++){
      if(strcmp("url_type", (*it)->name.c_str()) == 0){
	  //if(strcmp("1", (*it)->value)==0)
	    //type = ssds_xml::url_type::SSDS_BASEURL; //baseurl needs to be done in another way
	  if(strcmp("2", (*it)->value.c_str())==0)
	    type = LRO_MIRRORLISTURL;
	  else if(strcmp("3", (*it)->value.c_str())==0)
	    type = LRO_METALINKURL;
	  else
	    return;
      }
    }
    
    GError *tmp_err = NULL;
    
    // Download only this metadata
    char *download_list[] = { "primary", "filelists", "repomd", NULL};
    LrHandle *h = lr_handle_init();
    LrResult *r = lr_result_init();
    
    lr_handle_setopt(h, NULL, type, url_char);//"https://mirrors.fedoraproject.org/metalink?repo=updates-released-f21&arch=x86_64");
    lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);
    lr_handle_setopt(h, NULL, LRO_YUMDLIST, download_list);
    
    gboolean ret = lr_handle_perform(h, r, &tmp_err); 	
    
    char *destdir;
    lr_handle_getinfo(h, NULL, LRI_DESTDIR, &destdir);
    
    if (ret) {
      printf("Download successfull (Destination dir: %s)\n", destdir);
      std::vector<LrResult*>::iterator it = repo_info.end();
      repo_info.insert(it, r);
      
//       tmp_err = NULL;
//       LrYumRepo* repo = lr_yum_repo_init();
      
//       
//       ret = lr_result_getinfo(r, &tmp_err, LRR_YUM_REPO, &repo);
// 
//       if(ret){
    } else {
      fprintf(stderr, "Error encountered: %s\n", tmp_err->message);
      g_error_free(tmp_err);
    }
    
//     lr_result_free(r);
  //  lr_handle_free(h);
  }  
}