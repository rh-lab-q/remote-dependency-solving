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
  int repo_metadata::locate_repo_metadata_by_url()
  {
    for(ssds_xml::xml_node* it : this->urls){
      this->download_repo_metadata_by_url(*it);
    }
  }
  
  
  /*
   * 	Downloads repo metadata based on given url address
   */
  void repo_metadata::download_repo_metadata_by_url(ssds_xml::xml_node &repo_node)
  {
    char * url_char = (char *)repo_node.value.c_str();
    LrHandleOption type;
    
    std::cout << "download_repo_metadata_by_url: prijata url: " << url_char << std::endl;
    
    //find type of url in vector
    for(ssds_xml::xml_attr* it : repo_node.attributes){
      if(strcmp("url_type", it->name.c_str()) == 0){
	  //if(strcmp("1", (*it)->value)==0)
	    //type = ssds_xml::url_type::SSDS_BASEURL; //baseurl needs to be done in another way
	  if(strcmp("2", it->value.c_str())==0)
	    type = LRO_MIRRORLISTURL;
	  else if(strcmp("3", it->value.c_str())==0)
	    type = LRO_METALINKURL;
      }
    }
    
    GError *tmp_err = NULL;
    
    // Download only this metadata
    char *download_list[] = { "primary", "filelists", NULL};
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