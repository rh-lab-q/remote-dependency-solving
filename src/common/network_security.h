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

#ifndef _NETWORK_SECURITY_H
#define _NETWORK_SECURITY_H

#include "log_handler.h"
#include "mem_management.h"
#include "includes.h"
#include "cfg_parsing.h"
#include "errors.h"

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define USE_SSL 0	// 0 = do not use SSL, 1 = do use SSL

#define MY_CERT "cert/public.pem"		// public cetrificate file
#define MY_KEY "cert/private.pem"		// private key file
#define CERT_FILETYPE SSL_FILETYPE_PEM	// certificate file format
#define KEY_FILETYPE SSL_FILETYPE_PEM	// key file format
/* for information about SSL filetype definitions visit:
 * https://www.openssl.org/docs/manmaster/ssl/SSL_CTX_use_certificate.html
 */

#define PRINT_SSL_ERRORS_TO_STDERR 0

#define CIPHERS "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:\
 ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:\
 ECDHE-ECDSA-AES128-SHA:AES128-GCM-SHA256:AES128-SHA256:AES128-SHA:\
 DHE-DSS-AES128-GCM-SHA256:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES128-SHA256:\
 DHE-DSS-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA:PSK-AES128-CBC-SHA:\
 CAMELLIA128-SHA:DHE-RSA-CAMELLIA128-SHA:DHE-DSS-CAMELLIA128-SHA:ECDHE-RSA-AES256-GCM-SHA384:\
 ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:\
 ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:AES256-GCM-SHA384:AES256-SHA256:\
 AES256-SHA:DHE-DSS-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384:\
 DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA256:DHE-RSA-AES256-SHA:\
 DHE-DSS-AES256-SHA:PSK-AES256-CBC-SHA:CAMELLIA256-SHA:DHE-RSA-CAMELLIA256-SHA:\
 DHE-DSS-CAMELLIA256-SHA:ECDHE-RSA-DES-CBC3-SHA:ECDHE-ECDSA-DES-CBC3-SHA:\
 DES-CBC3-SHA:EDH-RSA-DES-CBC3-SHA:EDH-DSS-DES-CBC3-SHA:PSK-3DES-EDE-CBC-SHA:\
 ECDHE-RSA-RC4-SHA:ECDHE-ECDSA-RC4-SHA:RC4-SHA:PSK-RC4-SHA"

#ifdef __cplusplus
extern "C"{
#endif


/**
 * Function which uses either standard read(), or crypted read.
 * @param  fd		communication socket descriptor
 * @param  *buf		buffer for incoming message
 * @param  count  	maximum number of bytes to read
 * @return ssize_t	bytes read
 */
ssize_t rds_read(int fd, void *buf, size_t count);

/**
 * Function which uses either standard write(), or crypted write
 * @param  fd		communication socket descriptor
 * @param  *buf		buffer with outcomming message
 * @param  count  	maximum number of bytes to send
 * @return ssize_t	bytes sent
 */
ssize_t rds_write(int fd, const void *buf, size_t count);

/**
 * Shutdowns current ssl connection
 */
void crypted_connection_reset();

/**
 * creates	SSL connection
 * @param	fd		communication socket descriptor
 * @return	int		0 = success, -1 = fail
 */
int initialize_secure_connection(int fd);

/**
 * initialize SSL context
 * @return	SSL_CTX*		SSL context
 */
SSL_CTX* InitCTX(void);

/**
 * prints connection certificates
 * @param	ssl		ssl connection
 * @return	int		0 = success, -1 = fail
 */
int ShowCerts(SSL* ssl);

/**
 * Loads certificate and private key
 * @param	*ctx		ssl context
 * @param	*CertFile	string containing name of certificate file
 * @param	*KeyFile	string containing name of private key file
 * @return	int			0 = success, -1 = fail
 */
int LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile);


#ifdef __cplusplus
}
#endif

#endif /* _NETWORK_SECURITY_H */

