#include <iostream>
#include <string>
#include <hawkey/types.h>
#include <hawkey/sack.h>
#include <hawkey/packagelist.h>
#include <hawkey/query.h>
#include <hawkey/repo.h>
#include <hawkey/package.h>
#include <libhif/libhif.h>
#include <libhif/hif-package.h>
int main(){

	HySack sack = hy_sack_create(NULL, NULL, NULL, HY_MAKE_CACHE_DIR);
	if(hy_sack_load_system_repo(sack, NULL, HY_BUILD_CACHE) == 0){
		std::cout << "Nacteni rep je OK. Pocet rep: " << hy_sack_count(sack) << std::endl;
	}
	
	/* Loading repo metadata into sack */
	HyRepo repo = hy_repo_create("pokus");
        hy_repo_set_string(repo, HY_REPO_MD_FN, "/var/cache/dnf/x86_64/21/fedora/repodata/repomd.xml");
        hy_repo_set_string(repo, HY_REPO_PRIMARY_FN, "/var/cache/dnf/x86_64/21/fedora/repodata/e2a28baab2ea4632fad93f9f28144cda3458190888fdf7f2acc9bc289f397e96-primary.xml.gz");
        hy_repo_set_string(repo, HY_REPO_FILELISTS_FN, "/var/cache/dnf/x86_64/21/fedora/repodata/abb4ea5ccb9ad46253984126c6bdc86868442a4662dbcfa0e0f51b1bb209331e-filelists.xml.gz");
     
        if(hy_sack_load_yum_repo(sack, repo, 0) == 0)
          std::cout << "load_yum_repo v cajku, kontrolni pocet: " << hy_sack_count(sack) << std::endl;

	HyQuery query = hy_query_create(sack);
	const char * ask = "babl";	
	hy_query_filter(query, HY_PKG_NAME, HY_SUBSTR, ask);
	hy_query_filter_latest_per_arch(query, 1);

	HyPackageList plist = hy_packagelist_create();
	plist = hy_query_run(query);

	std::cout<< "Pocet nalezenych baliku: " << hy_packagelist_count(plist) << std::endl;
	
	HyPackage pkg;
	for( int i=0; i<hy_packagelist_count(plist); i++){
		pkg = hy_packagelist_get(plist,i);
		std::string name = hy_package_get_name(pkg),
			    arch = hy_package_get_arch(pkg);
		if(name.compare("babl") == 0  && arch.compare("x86_64") == 0){
 
		std::cout << name << "." << arch << std::endl;
		break;
	}
	}


	std::cout << hy_package_get_name(pkg);

	HifState state;
//		HyPackage pkg = ;
//	const gchar *name = "testovaci nazev";
//	hif_package_set_filename(pkg, name);
	const gchar *directory = "/home/jozkar/Plocha/download/";
	GError *error = NULL;	
	
	std::cout << "Testovaci priklad" << std::endl;
	 
	//HifSource *src = hif_package_get_source(pkg);
	gchar *ret = hif_package_download(pkg, directory, &state, &error);

	std::cout << ret << std::endl;
	return 0;
}
