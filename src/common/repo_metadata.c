/* Server side dependency solving - transfer of dependency solving from local machine to server when installing new packages
 * Copyright (C) 2015  Michal Ruprich, Josef Řídký
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "repo_handler.h"
#include "log_handler.h"


RepoMetadataList* repo_metadata_init() {
    RepoMetadataList* new;
    
    new = (RepoMetadataList*)malloc(sizeof(RepoMetadataList));
    new->files_locations = NULL;
    return new;
}


int locate_repo_metadata(/*JsonRead* json, */RepoInfoList* info_list, RepoMetadataList* meta_list) {
    guint len;

    len = g_slist_length(info_list->repoInfoList);
    for(guint i = 0; i < len; i++) {
        RepoInfo* repo;
        
         repo = (RepoInfo*)g_slist_nth_data(info_list->repoInfoList, i);

        if(!local_repo_metadata(repo, meta_list)) {
        //       rds_log(logDEBUG, "locate_repo_metadata inside if\n");
            download_repo_metadata_by_url(repo, meta_list);
            rds_log(logDEBUG, "locate_repo_metadata inside if after download\n");
        }

    }

    //TODO - update return value
    return 1;
}


int local_repo_metadata(RepoInfo* repos, RepoMetadataList* list) {
    char* local_path;
    GError *tmp_err = NULL;
    LrHandle *h;
    LrResult *r;
    char** handle_urls;
    gboolean ret;

    local_path = full_path_to_metadata(repos->name);
    h = lr_handle_init();
    r = lr_result_init();
    rds_log(logDEBUG, "%s\n", local_path);

    handle_urls = (char**)malloc(2*sizeof(char*));
    handle_urls[0] = local_path;
    handle_urls[1] = NULL;
    lr_handle_setopt(h, NULL, LRO_URLS, handle_urls);//look for repo locally
    lr_handle_setopt(h, NULL, LRO_LOCAL, (long)1);
    lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);

    ret = lr_handle_perform(h, r, &tmp_err);

    if(ret) {
        //     rds_log(logINFO,"Local metadata for %s found at %s. Using local copy.\n", repo->name, local_path);
        LrYumRepo* local_repo;
        MetadataFilesLoc* loc;
        
        local_repo = lr_yum_repo_init();
        lr_result_getinfo(r, &tmp_err, LRR_YUM_REPO, &local_repo);

        loc = (MetadataFilesLoc*)malloc(sizeof(MetadataFilesLoc));
        loc->repomd = local_path;
        loc->filelists = strdup(lr_yum_repo_path(local_repo,"filelists"));
        loc->primary = strdup(lr_yum_repo_path(local_repo,"primary"));
        loc->repo_name = strdup(repos->urls[0]);

        list->files_locations = g_slist_append(list->files_locations, loc);
        lr_yum_repo_free(local_repo);
        return 1;
    }

    //   rds_log(logINFO, "Local metadata for %s repo were not found at %s. Metadata will be downloaded.\n", repo->name, local_path);
    return 0;
}


char* full_path_to_metadata(char* repo_name) {
    const char* dest = "/tmp/ssds/";
    int length;
    char* full_path;

    length = strlen(dest) + strlen(repo_name);
    full_path = (char*)malloc((length+1)*sizeof(char));
    strncpy(full_path, dest, strlen(dest)+1);
    strncat(full_path, repo_name, strlen(repo_name));

    mkdir(dest, 0777);
    mkdir(full_path, 0777);

    return full_path;
}

int metadata_progress(G_GNUC_UNUSED void *data, double total, double now) {
    if(total > 0) {
        printf("\rDownloading repo: %s\t - %.0f%%", (char *) data, (now/total)*100);
        fflush(stdout);
    }
    return 0;
}

void download_repo_metadata_by_url(RepoInfo* repo, RepoMetadataList* list) {
    LrHandleOption type;
    GError *tmp_err = NULL;
    LrHandle *h;
    LrResult *r;
    char* full_path;
    gboolean ret;

    // Download only this metadata
    //char *download_list[] = { "primary", "filelists", NULL};
    h = lr_handle_init();
    r = lr_result_init();
    repo->urls[repo->count] = NULL;

    //find type of url in vector
    switch(repo->type) {
        case 1:
            type = LRO_URLS;
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

    full_path = full_path_to_metadata(repo->name);

    //lr_handle_setopt(h, NULL, LRO_YUMDLIST, download_list);
    lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);
    lr_handle_setopt(h, NULL, LRO_CONNECTTIMEOUT, (long)10);
    lr_handle_setopt(h, NULL, LRO_DESTDIR, full_path);
    lr_handle_setopt(h, NULL, LRO_PROGRESSCB, metadata_progress);  
    lr_handle_setopt(h, NULL, LRO_PROGRESSDATA, repo->name);
    //   printf("Performing handle on repo name: %s, repo type: %d\n", repo->name, repo->type);
    ret = lr_handle_perform(h, r, &tmp_err);
    printf("\n"); 
    char *destdir;
    lr_handle_getinfo(h, NULL, LRI_DESTDIR, &destdir);


    if (ret) {
        rds_log(logMESSAGE, "Metadata for %s - download successfull (Destination dir: %s).\n", repo->name, destdir);

        LrYumRepo* lrRepo;
        MetadataFilesLoc* loc;
        
        lrRepo = lr_yum_repo_init();
        lr_result_getinfo(r, &tmp_err, LRR_YUM_REPO, &lrRepo);
        //std::cout << lr_yum_repo_path(repo, "filelists") << std::endl;

        //     rds_log(logDEBUG, "Lr_result contains all the info now\n");

        loc = (MetadataFilesLoc*)malloc(sizeof(MetadataFilesLoc));

        loc->repomd = destdir;

        loc->filelists = strdup(lr_yum_repo_path(lrRepo,"filelists"));
        loc->primary = strdup(lr_yum_repo_path(lrRepo,"primary"));
        loc->repo_name = strdup(repo->urls[0]);
        //     
        //     printf("download_repo_metadata_by_url:\nrepomd: %s\nfilelists: %s\nprimary: %s\n", loc->repomd, loc->filelists, loc->primary);

        //     rds_log(logDEBUG, "Locations of downloaded files are ready\n");

        list->files_locations = g_slist_append(list->files_locations, loc);
        //     rds_log(logDEBUG, "Locations of files are in the list\n");
        lr_yum_repo_free(lrRepo);
        //     rds_log(logDEBUG, "download_repo_metadata_by_url after repo free\n");
    } 
    else {
        fprintf(stderr, "Error encountered: %s.\n", tmp_err->message);
        g_error_free(tmp_err);
    }

    //   rds_log(logDEBUG, "download_repo_metadata_by_url beffore free\n");
    //  lr_result_free(r);
    //  lr_handle_free(h);
    //   rds_log(logDEBUG, "download_repo_metadata_by_url after free\n");
}
