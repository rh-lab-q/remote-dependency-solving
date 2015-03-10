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
  void repo_metadata::by_url(ssds_xml::xml_node* repo_node, std::vector<LrYumRepo*> repo_info)
  {
    char * url_char = (char *)repo_node->value.c_str();
    LrHandleOption type;
    std::cout << "na zacatku by_url" << std::endl;
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
    std::cout << "po for v by_url" << std::endl;
#if 1
    GError *tmp_err = NULL;

    // Prepare list of variable substitutions
    //LrUrlVars *urlvars = NULL;
    //urlvars = lr_urlvars_set(urlvars, "releasever", "21");
    //urlvars = lr_urlvars_set(urlvars, "basearch", "x86_64");
    
    // Download only this metadata
    char *download_list[] = { "primary", "filelists", "repomd",NULL };
    LrHandle *h = lr_handle_init();
    LrResult *r = lr_result_init();
    
    std::cout << "pred setopt" << std::endl;
    lr_handle_setopt(h, NULL, type, url_char);//"https://mirrors.fedoraproject.org/metalink?repo=updates-released-f21&arch=x86_64");
    //"https://mirrors.fedoraproject.org/metalink?repo=updates-released-f21&arch=x86_64");
    lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);
    lr_handle_setopt(h, NULL, LRO_YUMDLIST, download_list);
    //lr_handle_setopt(h, NULL, LRO_VARSUB, urlvars);
    //lr_handle_setopt(h, NULL, LRO_PROGRESSCB, progress_callback);
    std::cout << "pred perform" << std::endl;
    gboolean ret = lr_handle_perform(h, r, &tmp_err); 	
    
    std::cout << "pred get info" << std::endl;
    char *destdir;
    lr_handle_getinfo(h, NULL, LRI_DESTDIR, &destdir);
    
    std::cout << "pred print" << std::endl;
    if (ret) {
      printf("Download successfull (Destination dir: %s)\n", destdir);
      tmp_err = NULL;
      LrYumRepo* repo = lr_yum_repo_init();
      ret = lr_result_getinfo(r, &tmp_err, LRR_YUM_REPO, &repo);

      if(ret){
	repo_info.push_back(repo);
	std::cout << "za getinfo" << std::endl;
	std::cout << "path filelists v get_url: " << lr_yum_repo_path(repo_info.at(0), "filelists") <<std::endl;
	std::cout << "path primary v get_url: " << lr_yum_repo_path(repo, "primary") <<std::endl;
      }
      else{
	std::cout << "Error encountered: " << tmp_err->message << std::endl;
	
      }
      
    } else {
      fprintf(stderr, "Error encountered: %s\n", tmp_err->message);
      g_error_free(tmp_err);
    }
    
    lr_result_free(r);
    lr_handle_free(h);
#endif
#if 0
    LrHandle *h = lr_handle_init();
    LrResult *r = lr_result_init();
    LrYumRepoMd *repomd;
    char *download_list[] = LR_YUM_HAWKEY;
    GError *tmp_err = nullptr;
    
    char* url_char = (char*)url.c_str();
    char *urls[]={url_char, nullptr};
    lr_handle_setopt(h, nullptr, LRO_URLS, urls);
    
    
    
    int ret = lr_handle_perform(h, r, &tmp_err);
    lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);
    lr_handle_setopt(h, NULL, LRO_YUMDLIST, download_list);
    
    
    
    lr_result_getinfo(r, &tmp_err, LRR_YUM_REPOMD, &repomd);
    
    
    
    if (!tmp_err) {
      printf("Repomd.xml contains:\n");
      for (GSList *elem = repomd->records; elem; elem = g_slist_next(elem)) {
	LrYumRepoMdRecord *rec = (LrYumRepoMdRecord *) elem->data;
	printf(" Type: %s\n", rec->type);
	printf(" Location href: %s\n", rec->location_href);
	if (rec->location_base)
	  printf(" Location base: %s\n", rec->location_base);
	if (rec->checksum)
	  printf(" Checksum: %s\n", rec->checksum);
	if (rec->checksum_type)
	  printf(" Checksum type: %s\n", rec->checksum_type);
	if (rec->checksum_open)
	  printf(" Checksum open: %s\n", rec->checksum_open);
	if (rec->checksum_open_type)
	  printf(" Checksum open type: %s\n", rec->checksum_open_type);
	if (rec->timestamp > 0)
	  printf(" Timestamp: %"G_GINT64_FORMAT"\n", rec->timestamp);
	if (rec->size > 0)
	  printf(" Size: %"G_GINT64_FORMAT"\n", rec->size);
	if (rec->size_open > 0)
	  printf(" Size open: %"G_GINT64_FORMAT"\n", rec->size_open);
	if (rec->db_version > 0)
	  printf(" Db version: %d\n", rec->db_version);
	  printf("------------------------------------------------\n");
      }
    } else {
      fprintf(stderr, "Error encountered: %s\n", tmp_err->message);
      g_error_free(tmp_err);
      //rc = EXIT_FAILURE;
    } 
#endif
    std::cout << "na konci metadat" << std::endl;
  }  
}