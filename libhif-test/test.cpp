#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <librepo/librepo.h>
#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmcli.h>
#include <rpm/rpmdb.h>

static void log_handler_cb(const gchar *log_domain G_GNUC_UNUSED, GLogLevelFlags log_level G_GNUC_UNUSED, const gchar *message, gpointer user_data G_GNUC_UNUSED){

	g_print("%s\n", message);
}

static int pc(G_GNUC_UNUSED void *data, double total, double now){
	if(total > 0){
		printf("\r%s\t%.0f%%",data, (now/total)*100);
        	fflush(stdout);
	}
	return 0;
}
static int ec(G_GNUC_UNUSED void *data, LrTransferStatus s, const char *msg){
	if(s == LR_TRANSFER_SUCCESSFUL){
		printf("\r%s\t%s\n",data,"100% - Downloaded.");
        }else{
		printf("\r%s\t%s\n",data,msg);
	}
	fflush(stdout);
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
	system("ls");	
/*	char *urls[] = {"http://why.lri.fr/", "https://mirrors.fedoraproject.org/metalink?repo=fedora-21&arch=x86_64","https://mirrors.fedoraproject.org/metalink?repo=updates-released-f21&arch=x86_64",NULL};
	char *name = "w/why-xemacs-el-2.34-13.fc21.noarch.rpm";
	h = lr_handle_init();
	lr_handle_setopt(h, NULL, LRO_URLS, urls);
	lr_handle_setopt(h, NULL, LRO_REPOTYPE, LR_YUMREPO);
	lr_handle_setopt(h, NULL, LRO_PROGRESSCB, pc);
	// Prepare list of target

	target = lr_packagetarget_new_v2(h, name, "./rpm/", LR_CHECKSUM_UNKNOWN, NULL, 0, NULL, TRUE, pc, name, ec, NULL, &error);
	packages = g_slist_append(packages, target);
	target = lr_packagetarget_new(h, "l/libwmf-lite-0.2.8.4-43.fc21.x86_64.rpm", "./rpm/", LR_CHECKSUM_UNKNOWN, NULL, 0, NULL, TRUE, NULL, NULL, &error);
        packages = g_slist_append(packages, target);
target = lr_packagetarget_new(h, "l/libwmf-0.2.8.4-43.fc21.x86_64.rpm", "./rpm/", LR_CHECKSUM_UNKNOWN, NULL, 0, NULL, TRUE, NULL, NULL, &error);
        packages = g_slist_append(packages, target);
target = lr_packagetarget_new(h, "b/babl-0.1.12-2.fc21.x86_64.rpm", "./rpm/", LR_CHECKSUM_UNKNOWN, NULL, 0, NULL, TRUE, NULL, NULL, &error);
        packages = g_slist_append(packages, target);
target = lr_packagetarget_new(h, "t/tbb-4.3-1.20141204.fc21.x86_64.rpm", "./rpm/", LR_CHECKSUM_UNKNOWN, NULL, 0, NULL, TRUE, NULL, NULL, &error);
        packages = g_slist_append(packages, target);
target = lr_packagetarget_new(h, "a/atlas-3.10.1-18.fc21.x86_64.rpm", "./rpm/", LR_CHECKSUM_UNKNOWN, NULL, 0, NULL, TRUE, NULL, NULL, &error);
        packages = g_slist_append(packages, target);
	// Download all packages	
	ret = lr_download_packages(packages, LR_PACKAGEDOWNLOAD_FAILFAST, &error);
	if(!ret){
		fprintf(stderr, "Error: %d: %s\n", error->code, error->message);
		rc = EXIT_FAILURE;
		g_error_free(error);
	}	
 */
	// Check statuses

	// RPM init  
	// TODO: need to init rpmdb connection
	Header hdr;
        char namen[] = "/var/cache/ssds/packages/install/gimp-2.8.14-3.fc21.x86_64.rpm";
	rpmReadConfigFiles(NULL,NULL);
        rpmts ts = rpmtsCreate();
	rpmprobFilterFlags ignoreSet = 0;
	rpmtsSetRootDir(ts, NULL);
//	for(GSList *elem = packages; elem; elem = g_slist_next(elem)){
//                LrPackageTarget *t = (LrPackageTarget *)elem->data;
//                printf("%s: %s\n", t->local_path, t->err ? t->err : "OK");
		// installing package
          //      if(!t->err){
                        FD_t fd = (FD_t) fopen(namen,"r.ufdio");
                        printf("%d\n", fd);
			char *msg = NULL;
//                        int rr = rpmReadPackageFile(ts,fd,namen,&hdr);
                        int rr = rpmReadHeader(ts,fd,&hdr,&msg);
			printf("Package is %s. %d %s\n",rr!=RPMRC_OK? "corrupted":"ok",rr,msg);
			int upgrade = 0;
			//rpmRelocation rel = NULL;
			int r = rpmtsAddInstallElement(ts, hdr,namen ,upgrade,0);
                printf("add install element %s %d.\n", r==RPMRC_OK?"ok":"error", r);
		//}
        //}
	rpmtsi tsi = rpmtsiInit(ts);
	if(!rpmtsCheck(ts)){
		if(rpmtsOrder(ts)){
			printf("ERROR: some packages cannot be ordered\n");
		}else{
			int result = rpmtsRun(ts, NULL, ignoreSet);
			printf("rpmts run %s.\n", result==RPMRC_OK?"ok":"error");
		}
	}
	// Clean up
	rpmtsClean(ts);
	rpmtsFree(ts);
	g_slist_free_full(packages, (GDestroyNotify) lr_packagetarget_free);
	lr_handle_free(h);

	return rc;
}
