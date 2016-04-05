#include "detect_missing_repos.h"

int check_for_missing_repos() {
    #ifdef DEBUG
        printf("DEBUG: in check_for_missing_repos\n");
    #endif
    char **req_repos;
    FILE* reposFD;
    int req_repo_count = 0;
    int alloc_for_names = 10;

    req_repos = (char**)rds_malloc(alloc_for_names * sizeof(char*));
    /* Load names of required repos from file */
    reposFD = fopen(REPO_LIST_FILE, "r");
    if (reposFD == NULL) {
        syslog(LOG_ERR, "Unable to open file with required repo list");
        fprintf(stderr, "Unable to open required_repos_list file.\n");
        return -1;
    }
    for(;;) {
        char tc;

        if (alloc_for_names <= req_repo_count) {
            alloc_for_names += 5;
            req_repos = (char**)rds_realloc(req_repos, alloc_for_names * sizeof(char*));
        }
        req_repos[req_repo_count] = file_read_value(reposFD, 0);

        if (strlen(req_repos[req_repo_count]) >= REPO_NAME_MIN_LENGTH)
            req_repo_count++;

        tc = fgetc(reposFD);
        if (tc == EOF)
            break;
        else
            fseek(reposFD, SEEK_CUR, -1);
    }
    fclose(reposFD);


    /* Load list of repos in /etc/yum.repos.d/ */
    DIR* repo_dir;
    struct dirent* dirs;
    alloc_for_names = 10;
    int local_repo_count = 0;
    char **local_repos;

    repo_dir = opendir(LOCAL_REPOS_LOCATION);
    local_repos = (char**)rds_malloc(alloc_for_names * sizeof(char*));

    while ((dirs = readdir(repo_dir)) != NULL) {
        if (alloc_for_names <= local_repo_count) {
            alloc_for_names += 5;
            local_repos = (char**)rds_realloc(local_repos, alloc_for_names * sizeof(char*));
        }
        local_repos[local_repo_count] = dirs->d_name;
        local_repo_count++;
    }


    /* testing if all of required repos are present in local repos */
    int missing_repos = 0;
    int found = 0;

    for (int i1 = 0; i1 < req_repo_count; i1++) {
        for (int i2 = 0; i2 < local_repo_count; i2++) {
            if ((strlen(req_repos[i1]) == strlen(local_repos[i2])) && (strncmp(req_repos[i1], local_repos[i2], strlen(req_repos[i1])) == 0)) {
                found = 1;
                break;
            }
        }
        if (found != 1) {
            syslog(LOG_WARNING, "Missing repository in RDS. This might result in dependency solving problems");
            printf("MESSAGE: Missing repository [%s], ssds might not work correctly\n", req_repos[i1]);
            missing_repos++;
        }
    }


    closedir(repo_dir);
    for (int i1 = 0; i1 < req_repo_count; i1++)
        free(req_repos[i1]);

    free(req_repos);
    free(local_repos);
    return missing_repos;
}
