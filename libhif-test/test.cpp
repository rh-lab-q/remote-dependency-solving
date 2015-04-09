#include <iostream>
#include <libhif/libhif.h>
int main(){

	struct HifState *state;
	struct HyPackage *pkg;
	hif_package_set_filename(&pkg, "testovaci nazev");
	const gchar *directory = "/home/jozkar/Plocha/download/";
	GError **error;	
	
	std::cout << "Testovaci priklad" << std::endl;
	
//	gchar *ret = hif_package_download(pkg, directory, state, error);

	return 0;
}
