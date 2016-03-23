/* Server side dependency solving 
 * transfer of dependency solving from local machine to server when installing new packages
 * 
 * Copyright (C) 2015 Michal Ruprich, Josef Řídký, Walter Scherfel, Šimon Matěj
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

#include "globvar.h"
#include "errors.h"
#include "packages.h"
#include "log_handler.h"

int add_to_transaction(rpmts ts, char *pkg, int action) {
    FD_t fd;
    Header hdr;
    int rc = 0;

    /* Read package header */
    fd = Fopen(pkg, "r.ufdio");
    if (fd == NULL) {
        rds_log(logERROR, "Unable to open file %s.\n", pkg);
        return 1;
    }
    rds_log(logDEBUG, "File %s is opened.\n", pkg);

    rc = rpmReadPackageFile(ts, fd, pkg, &hdr);

    if (rc != RPMRC_OK) {
        rds_log(logERROR,"Unable to read package %s.\n", pkg);
        return rc;
    }

    rds_log(logDEBUG, "Package is ok.\n");

    /* Add it to the transaction set */
    rc = rpmtsAddInstallElement(ts, hdr, (fnpyKey) pkg, action, 0);

    if (rc)
        rds_log(logERROR ,"Error adding %s to transaction.\n", pkg);

    rds_log(logDEBUG, "Package added to transaction with code %d.\n", rc);

    headerFree(hdr);
    Fclose(fd);
    return rc;
}

int add_to_erase(rpmts ts, char *pkg) {
    Header hdr;
    rpmdbMatchIterator mi;
    int rc = OK;

    /* Locate the package and add for erasure */
    mi = rpmtsInitIterator(ts, (rpmTag)RPMDBI_LABEL, pkg, 0);
    while ((hdr = rpmdbNextIterator(mi)) != NULL) {
        int recOffset;
        
        recOffset = rpmdbGetIteratorOffset(mi);
            if (recOffset) {
            rc = rpmtsAddEraseElement(ts, hdr, recOffset);
            if (rc) 
                rds_log(logERROR, "Error adding %s to transaction.\n", pkg);
        }
    }
    mi = rpmdbFreeIterator(mi);
    return rc;
}
