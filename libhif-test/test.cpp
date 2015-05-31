#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <librepo/librepo.h>
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmio.h>

static void log_handler_cb(const gchar *log_domain G_GNUC_UNUSED, GLogLevelFlags log_level G_GNUC_UNUSED, const gchar *message, gpointer user_data G_GNUC_UNUSED){

	g_print("%s\n", message);
}

int main(){

	int rc = EXIT_SUCCESS;
	gboolean ret;
	LrHandle *h;
	GSList *packages = NULL;
	LrPackageTarget *target;
	GError *error = NULL;

	// setup logging (optional)
	
//	g_log_set_handler("librepo-test", G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_WARNING, log_handler_cb, NULL);

	// prepare handle
	
	char *urls[] = {"http://beaker-project.org/yum/client-testing/Fedora19/", NULL};
	h = lr_handle_init();
	lr_handle_setopt(h, NULL, LRO_URLS, urls);
	lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);

	// Prepare list of target

	target = lr_packagetarget_new(h, "beaker-0.15.2-1.fc18.src.rpm", "./rpm/", LR_CHECKSUM_UNKNOWN, NULL, 0, NULL, TRUE, NULL, NULL, &error);
	packages = g_slist_append(packages, target);

	
	// Download all packages	

	ret = lr_download_packages(packages, LR_PACKAGEDOWNLOAD_FAILFAST, &error);
	
	if(!ret){
		fprintf(stderr, "Error: %d: %s\n", error->code, error->message);
		rc = EXIT_FAILURE;
		g_error_free(error);
	}	
 
	// Check statuses
	
	for(GSList *elem = packages; elem; elem = g_slist_next(elem)){
                LrPackageTarget *t = (LrPackageTarget *)elem->data;
                printf("%s: %s\n", t->local_path, t->err ? t->err : "OK");
		// installing package
                if(!t->err){
                        FD_t fd = (FD_t)fopen(t->local_path,"r");
                        rpmRC result;
                        rpmts ts = rpmtsCreate();

                        result = rpmInstallSourcePackage(ts, fd, NULL, NULL);
                }
        }
	
	// Clean up

	g_slist_free_full(packages, (GDestroyNotify) lr_packagetarget_free);
	lr_handle_free(h);

	return rc;
}
