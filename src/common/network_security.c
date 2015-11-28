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

#include "network_security.h"


/*
DO NOT CHANGE FOLLOWING VALUES,
IF YOU WANT TO DISABLE SSL, DO SO BY CHANGING USE_SSL IN THE .h FILE
*/
int _ssl_connected = 0;					// connection secured
SSL_CTX *ctx = NULL;							// SSL context
SSL *ssl = NULL;								// SSL connection


ssize_t ssds_read(int fd, void *buf, size_t count)
{
	ssds_log(logDEBUG, "Reading max %d bytes of data\n", count);
	int res = -1;
	if (USE_SSL == 0) // unencrypted communication
	{
		ssds_log(logDEBUG, "Using unencrypted read\n");
		res = read(fd, buf, count);
	}
	else  // encrypted communication
	{
		ssds_log(logDEBUG, "Using encrypted read\n");
		if (_ssl_connected == 0) // initialize connection
		{
			if (initialize_secure_connection(fd) == -1)
			{
				if (ssl != NULL) SSL_free(ssl);
				if (ctx != NULL) SSL_CTX_free(ctx);
				return -1;
			}
			else
			{
				ssds_log(logMESSAGE, "SSL successfully connected\n");
				_ssl_connected = 1;
			}
		}
		res = SSL_read(ssl, buf, count);
		if (res == -1)
		{
			ssds_log(logERROR, "SSL read failed\n");
			if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
		}
	}
	ssds_log(logDEBUG, "Read %d bytes\n", res);
	return res;
}


ssize_t ssds_write(int fd, const void *buf, size_t count)
{
	ssds_log(logDEBUG, "Writing %d bytes of data\n", count);
	int res = -1;
	if (USE_SSL == 0)
	{
		ssds_log(logDEBUG, "Using unencrypted write\n");
		res = write(fd, buf, count);
	}
	else
	{
		ssds_log(logDEBUG, "Using encrypted write\n");
		if (_ssl_connected == 0) // not connected
		{
			if (initialize_secure_connection(fd) == -1)
			{
				if (ssl != NULL) SSL_free(ssl);
				if (ctx != NULL) SSL_CTX_free(ctx);
				return -1;
			}
			else
			{
				ssds_log(logMESSAGE, "SSL successfully connected\n");
				_ssl_connected = 1;
			}
		}
		res = SSL_write(ssl, buf, count);
		if (res == -1)
		{
			ssds_log(logERROR, "SSL write failed\n");
			if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
		}
	}
	ssds_log(logDEBUG, "Written %d bytes\n", res);
	return res;
}


int initialize_secure_connection(int fd)
{
	ssds_log(logDEBUG, "Init SSL\n");
	sleep(5);
	ctx = InitCTX();
	if (ctx == NULL)
	{
		return -1;
	}

	//server loads certificate and key
	#ifdef __CASE_SERVER
	ssds_log(logDEBUG, "Server is loading certificates\n");
	if (LoadCertificates(ctx, MY_CERT, MY_KEY) == -1)
	{
		ssds_log(logERROR, "SSL fail (server load certs)\n");
		if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
		return -1;
	}
	else
	{
		ssds_log(logMESSAGE, "SSL certificates successfully loaded\n");
	}
	#endif

	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, fd);

	//client connects to server
	#ifdef __CASE_CLIENT
	if (SSL_connect(ssl) == -1)
	{
		ssds_log(logERROR, "SSL fail (client connect)\n");
		if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
		return -1;
	}
	#endif

	//server accepts client
	#ifdef __CASE_SERVER
	if (SSL_accept(ssl) == -1)
	{
		ssds_log(logERROR, "SSL fail (server accept)\n");
		if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
		return -1;
	}
	#endif

	if (ShowCerts(ssl) == -1)
	{
		ssds_log(logERROR, "SSL fail (no peer certificates avalaible)\n");
		if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
		return -1;
	}
	return 0;
}


SSL_CTX* InitCTX(void)
{ 
	const SSL_METHOD *method;

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	#ifdef __CASE_CLIENT
	method = TLSv1_2_client_method();
	#endif
	#ifdef __CASE_SERVER
	method = TLSv1_2_server_method();
	#endif
	ctx = SSL_CTX_new(method);
	if ( ctx == NULL )
	{
		if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
	}
	if (SSL_CTX_set_cipher_list(ctx, CIPHERS) != 1)
	{
		ssds_log(logERROR, "Couldnt use any ciphers\n");
		return NULL;
	}
	return ctx;
}


int ShowCerts(SSL* ssl)
{
	X509 *cert;
	char *line;
	cert = SSL_get_peer_certificate(ssl);
	if ( cert != NULL )
	{
		ssds_log(logDEBUG, "Peer certificate:\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		ssds_log(logDEBUG, "Subject:\n%s\n", line);
		free(line);
		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		ssds_log(logDEBUG, "Issuer:\n%s\n", line);
		free(line);
		X509_free(cert);
		return 0;
	}
	else
		ssds_log(logERROR, "No peer certificate avalaible.\n");
	return -1;
}


int LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    if (SSL_CTX_use_certificate_file(ctx, CertFile, CERT_FILETYPE) <= 0)
    {
        ssds_log(logERROR, "Could not load certificate\n");
        if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
        return -1;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, KEY_FILETYPE) <= 0)
    {
        ssds_log(logERROR, "Could not load private key\n");
        if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
        return -1;
    }
    if (!SSL_CTX_check_private_key(ctx))
    {
        ssds_log(logERROR, "Certificate and key do not match\n");
        if (PRINT_SSL_ERRORS_TO_STDERR) ERR_print_errors_fp(stderr);
        return -1;
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);
    return 0;
}

void crypted_connection_reset()
{
	if (ssl != NULL) SSL_free(ssl);
	if (ctx != NULL) SSL_CTX_free(ctx);
	_ssl_connected = 0;
}