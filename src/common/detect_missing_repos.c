#include "detect_missing_repos.h"

int check_for_missing_repos()
{
	char **req_repos;

  FILE* reposFD;
  int req_repo_count = 0;
  int alloc_for_names = 10;
  req_repos = (char**)ssds_malloc(alloc_for_names * sizeof(char*));


  /* Load names of required repos from file */

  reposFD = fopen(REPO_LIST_FILE, "r");
  if (reposFD == NULL)
  {
    ssds_log(logDEBUG, "Could not open required_repos_list file.\n");
    return -1;
  }
  while (1)
  {
  	if (alloc_for_names <= req_repo_count)
  	{
  		alloc_for_names += 5;
  		req_repos = (char**)ssds_realloc(req_repos, alloc_for_names * sizeof(char*));
  	}
  	req_repos[req_repo_count] = file_read_value(reposFD, 0);

  	if (strlen(req_repos[req_repo_count]) >= REPO_NAME_MIN_LENGTH)
  	{
  		req_repo_count++;
  	}

  	char tc = fgetc(reposFD);
  	if (tc == EOF)
  	{
  		break;
  	}
  	else
  	{
  		fseek(reposFD, SEEK_CUR, -1);
  	}
  }
  fclose(reposFD);


  /* Load list of repos in /etc/yum.repos.d/ */

	DIR* repo_dir = opendir(LOCAL_REPOS_LOCATION);
	struct dirent* dirs;
	alloc_for_names = 10;
	int local_repo_count = 0;
	char **local_repos;
	local_repos = (char**)ssds_malloc(alloc_for_names * sizeof(char*));

  while ((dirs = readdir(repo_dir)) != NULL)
  {
  	if (alloc_for_names <= local_repo_count)
  	{
  		alloc_for_names += 5;
  		local_repos = (char**)ssds_realloc(local_repos, alloc_for_names * sizeof(char*));
  	}
  	local_repos[local_repo_count] = dirs->d_name;
  	local_repo_count++;
  }


  /* testing if all of required repos are present in local repos */

  int missing_repos = 0;
  int found = 0;

  for (int i1 = 0; i1 < req_repo_count; i1++)
  {
  	for (int i2 = 0; i2 < local_repo_count; i2++)
  	{
  		if ((strlen(req_repos[i1]) == strlen(local_repos[i2])) && (strncmp(req_repos[i1], local_repos[i2], strlen(req_repos[i1])) == 0))
  		{
  			found = 1;
  			break;
  		}
  	}
		if (found != 1)
		{
			ssds_log(logMESSAGE, "Missing repository [%s], ssds might not work correctly\n", req_repos[i1]);
      missing_repos++;
		}
  }


  closedir(repo_dir);
  for (int i1 = 0; i1 < req_repo_count; i1++)
  {
    ssds_free(req_repos[i1]);
  }
  ssds_free(req_repos);
  ssds_free(local_repos);
  return missing_repos;
}