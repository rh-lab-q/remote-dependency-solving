#include "repo_handler_2.h"


SsdsRepoMetadataList* ssds_repo_metadata_init()
{
  SsdsRepoMetadataList* new = (SsdsRepoMetadataList*) malloc(sizeof(SsdsRepoMetadataList));
  new->files_locations = NULL;
  return new;
}


int ssds_locate_repo_metadata(SsdsJsonRead* json, SsdsRepoInfoList* info_list, SsdsRepoMetadataList* meta_list)
{
  guint len = g_slist_length(info_list->repoInfoList);
  guint i;
  for(i=0;i<len;i++){
    SsdsRepoInfo* repo = (SsdsRepoInfo*)g_slist_nth_data(info_list->repoInfoList, i);
    
    if(!local_repo_metadata(repo, meta_list))
      download_repo_metadata_by_url(repo, meta_list);
    
  }
}


int local_repo_metadata(SsdsRepoInfo* repo, SsdsRepoMetadataList* list)
{
  char* local_path=full_path_to_metadata(repo->name);
  GError *tmp_err = NULL;
  LrHandle *h = lr_handle_init();
  LrResult *r = lr_result_init();
  
  char** handle_urls=(char**)malloc(2*sizeof(char*));
  handle_urls[0]=local_path;
  handle_urls[1]=NULL;
  lr_handle_setopt(h, NULL, LRO_URLS, handle_urls);
  lr_handle_setopt(h, NULL, LRO_LOCAL, (long)1);
  lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);
  
  gboolean ret = lr_handle_perform(h, r, &tmp_err);
  
  if(ret)
  {
    printf("Local metadata for %s found at %s. Using local copy.\n", repo->name, local_path);
    LrYumRepo* repo = lr_yum_repo_init();
    lr_result_getinfo(r, &tmp_err, LRR_YUM_REPO, &repo);
    
    SsdsMetadataFilesLoc* loc = (SsdsMetadataFilesLoc*)malloc(sizeof(SsdsMetadataFilesLoc));
    loc->repomd = local_path;
    loc->filelists = (char*)lr_yum_repo_path(repo,"filelists");
    loc->primary = (char*)lr_yum_repo_path(repo,"primary");
    
    list->files_locations = g_slist_append(list->files_locations, loc);
//     this->files_locations.push_back(loc);
    return 1;
  }
    
  printf("Local metadata for %s repo were not found at %s. Metadata will be downloaded.\n", repo->name, local_path);
  return 0;
}


char* full_path_to_metadata(char* repo_name)
{
  struct stat sb;
  const char* dest="/tmp/ssds/";
  int length=strlen(dest)+strlen(repo_name);
  char* full_path=(char*)malloc((length+1)*sizeof(char));
  
  strncpy(full_path, dest, strlen(dest)+1);
  strncat(full_path, repo_name, strlen(repo_name));
  
  mkdir(dest, 0777);
  mkdir(full_path, 0777);
  
  return full_path;
}


void download_repo_metadata_by_url(SsdsRepoInfo* repo, SsdsRepoMetadataList* list)
{
  LrHandleOption type;
  GError *tmp_err = NULL;
  
  // Download only this metadata
  //char *download_list[] = { "primary", "filelists", NULL};
  LrHandle *h = lr_handle_init();
  LrResult *r = lr_result_init();
  repo->urls[repo->count]=NULL;
  
  //find type of url in vector
  switch(repo->type)
  {
    case 1:
      type=LRO_URLS;
      lr_handle_setopt(h, NULL, type, repo->urls);
      break;
    case 2:
      type = LRO_MIRRORLISTURL;
      lr_handle_setopt(h, NULL, type, repo->urls[0]);
      break;
    case 3:
      type = LRO_METALINKURL;
      lr_handle_setopt(h, NULL, type, repo->urls[0]);
      break;
  }

  char* full_path=full_path_to_metadata(repo->name);
  
  //lr_handle_setopt(h, NULL, LRO_YUMDLIST, download_list);
  lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);
  lr_handle_setopt(h, NULL, LRO_CONNECTTIMEOUT, (long)10);
  lr_handle_setopt(h, NULL, LRO_DESTDIR, full_path);
  
  printf("Performing handle on repo name: %s, repo type: %d\n", repo->name, repo->type);
  gboolean ret = lr_handle_perform(h, r, &tmp_err);
  
  char *destdir;
  lr_handle_getinfo(h, NULL, LRI_DESTDIR, &destdir);
  
  
  if (ret) {
    printf("Download successfull (Destination dir: %s)\n", destdir);
    
    LrYumRepo* lrRepo = lr_yum_repo_init();
    lr_result_getinfo(r, &tmp_err, LRR_YUM_REPO, &lrRepo);
    //std::cout << lr_yum_repo_path(repo, "filelists") << std::endl;
    
    SsdsMetadataFilesLoc* loc = (SsdsMetadataFilesLoc*)malloc(sizeof(SsdsMetadataFilesLoc));
    
    loc->repomd = destdir;
    loc->filelists = lr_yum_repo_path(lrRepo,"filelists");
    loc->primary = lr_yum_repo_path(lrRepo,"primary");
    
    list->files_locations = g_slist_append(list->files_locations, loc);
//     this->files_locations.push_back(loc);

  } else {
    fprintf(stderr, "Error encountered: %s\n", tmp_err->message);
    g_error_free(tmp_err);
  }
  
  lr_result_free(r);
  lr_handle_free(h);
}