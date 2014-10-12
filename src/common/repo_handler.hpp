#ifndef _REPO_HANDLER_HPP
#define _REPO_HANDLER_HPP
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <dirent.h>
#include <string.h>

namespace ssds_repo{
	class parse_repo{
	public:
		parse_repo()
		{
			DIR *directory;
			struct dirent *entry;
			
			directory = opendir("/etc/yum.repos.d/");
			
			if (directory == NULL) {
				perror("opendir");
			}
			 
			while((entry = readdir(directory))){
				if(strcmp(entry->d_name,".") !=0 && strcmp(entry->d_name,"..") !=0){//need to get rid of . and ..
					std::ifstream file;
					file.open(entry->d_name);
					std::ifstream* filePtr = &file;
					
					repolist.push_back(filePtr);
				}
			}
		}
		
		void get_url()
		{
			for(std::vector<std::ifstream*>::iterator it = repolist.begin(); it != repolist.end(); it++){
				
			}
		}
		
	public:
		std::vector<std::ifstream*> repolist;//list of file descriptors pointing to .repo files
		
	};
}


#endif