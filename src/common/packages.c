#include "packages.h"

int ssds_add_to_transaction(rpmts ts, char *pkg, int action)
{

	FD_t fd;
	Header hdr;
	int rc = 0;

	/* Read package header */
	fd = Fopen(pkg, "r.ufdio");
	if (fd == NULL) {
		ssds_log(logERROR, "Unable to open file %s.\n", pkg);
		return 1;
	}
	ssds_log(logDEBUG, "File %s is opened.\n", pkg);

	rc = rpmReadPackageFile(ts, fd, pkg, &hdr);

	if (rc != RPMRC_OK) {
		ssds_log(logERROR,"Unable to read package %s.\n", pkg);
		return rc;
	}

        ssds_log(logDEBUG, "Package is ok.\n");

	/* Add it to the transaction set */
	rc = rpmtsAddInstallElement(ts, hdr, (fnpyKey) pkg, action, 0);

	if (rc) {
		ssds_log(logERROR ,"Error adding %s to transaction.\n", pkg);
	}
 
	ssds_log(logDEBUG, "Package added to transaction with code %d.\n", rc);

	headerFree(hdr);
	Fclose(fd);
	return rc;
}

int ssds_add_to_erase(rpmts ts, char *pkg){

	Header hdr;
	rpmdbMatchIterator mi;
	int rc = OK;

	/* Locate the package and add for erasure */
	mi = rpmtsInitIterator(ts, (rpmTag)RPMDBI_LABEL, pkg, 0);
	while ((hdr = rpmdbNextIterator(mi)) != NULL) {
		int recOffset = rpmdbGetIteratorOffset(mi);
		if (recOffset) {
			rc = rpmtsAddEraseElement(ts, hdr, recOffset);
			if (rc) 
				ssds_log(logERROR, "Error adding %s to transaction.\n", pkg);

		}
	}
	mi = rpmdbFreeIterator(mi);
	return rc;
}
