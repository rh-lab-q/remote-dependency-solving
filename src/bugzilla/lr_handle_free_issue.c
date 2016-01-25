#include <stdio.h>

//LIBREPO
#include <librepo/repoconf.h>
#include <librepo/librepo.h>

int main()
{
  LrHandleOption type;
  GError *tmp_err = NULL;
  
  // Download only this metadata
  //char *download_list[] = { "primary", "filelists", NULL};
  LrHandle *h = lr_handle_init();
  LrResult *r = lr_result_init();
  //repo->urls[repo->count] = NULL;
  
  //find type of url in vector
//   switch(repo->type)
//   {
//     case 1:
//       type = LRO_URLS;
//       lr_handle_setopt(h, NULL, type, repo->urls);
//       break;
//     case 2:
//       type = LRO_MIRRORLISTURL;
//       lr_handle_setopt(h, NULL, type, repo->urls[0]);
//       break;
//     case 3:
//       type = LRO_METALINKURL;
//       lr_handle_setopt(h, NULL, type, repo->urls[0]);
//       break;
//   }
  char* full_path = "/tmp/ssds/pokus";
  char* urls[2] = {"http://copr-be.cloud.fedoraproject.org/results/dvratil/plasma-5/fedora-21-x86_64/", NULL};
  lr_handle_setopt(h, NULL, LRO_URLS, urls);
  lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);
  lr_handle_setopt(h, NULL, LRO_CONNECTTIMEOUT, (long)10);
  lr_handle_setopt(h, NULL, LRO_DESTDIR, full_path);
//   lr_handle_setopt(h, NULL, LRO_PROGRESSCB, metadata_progress);  
//   lr_handle_setopt(h, NULL, LRO_PROGRESSDATA, repo->name);
  
  gboolean ret = lr_handle_perform(h, r, &tmp_err);
  char *destdir;
  lr_handle_getinfo(h, NULL, LRI_DESTDIR, &destdir);
  
  
  if (ret) {
    printf("Metadata download successfull (Destination dir: %s).\n", destdir);
    
    LrYumRepo* lrRepo = lr_yum_repo_init();
    lr_result_getinfo(r, &tmp_err, LRR_YUM_REPO, &lrRepo);
    
//     SsdsMetadataFilesLoc* loc = (SsdsMetadataFilesLoc*)ssds_malloc(sizeof(SsdsMetadataFilesLoc));
    
//     loc->repomd = destdir;
    printf("Repomd is in %s/repomd.xml.\n", destdir);
//     loc->filelists = strdup(lr_yum_repo_path(lrRepo,"filelists"));
    printf("Filelists are in %s.\n", lr_yum_repo_path(lrRepo,"filelists"));
//     loc->primary = strdup(lr_yum_repo_path(lrRepo,"primary"));
    printf("Primary is in %s.\n", lr_yum_repo_path(lrRepo,"primary"));
//     loc->repo_name = strdup(repo->urls[0]);
    
//     list->files_locations = g_slist_append(list->files_locations, loc);
    lr_yum_repo_free(lrRepo);
    printf("lr_yum_repo_free went OK\n");
  } else {
    fprintf(stderr, "Error encountered: %s.\n", tmp_err->message);
    g_error_free(tmp_err);
  }
  
 lr_result_free(r);
 printf("lr_result_free went OK\n");
 lr_handle_free(h);
 printf("lr_handle_free went OK\n");
  
  
  
  
}