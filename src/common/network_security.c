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


int _use_secure_connection = 0;	// 0 = do not use SSL, 1 = do use SSL


int _ssl_connected = 0;			// Leave it at 0!
SSL_CTX *ctx;					// SSL context
SSL *ssl;						// SSL connection


ssize_t ssds_read(int fd, void *buf, size_t count)
{
	sleep(1); ssds_log(logDEBUG, "Reading max %d bytes of data\n", count);
	int res = -1;
	if (_use_secure_connection == 0)
	{
		sleep(1); ssds_log(logDEBUG, "Using nonciphered read\n");
		res = read(fd, buf, count);
	}
	else
	{
		sleep(1); ssds_log(logDEBUG, "Using ciphered read\n");
		if (_ssl_connected == 0) // initialize connection
		{
			if (initialize_secure_connection(fd) == -1)
			{
				return ssds_read(fd, buf, count);
				// SSL failed, trying non-SSL
			}
			else
			{
				sleep(1); ssds_log(logMESSAGE, "SSL successfully connected\n");
				_ssl_connected = 1;
			}
		}
		res = SSL_read(ssl, buf, count);
		if (res == -1)
		{
			sleep(1); ssds_log(logERROR, "SSL read failed\n");
			ERR_print_errors_fp(stderr);
		}
	}
	sleep(1); ssds_log(logDEBUG, "Read %d bytes\n", res);
	return res;
}


ssize_t ssds_write(int fd, const void *buf, size_t count)
{
	sleep(1); ssds_log(logDEBUG, "Writing %d bytes of data\n", count);
	int res = -1;
	if (_use_secure_connection == 0)
	{
		sleep(1); ssds_log(logDEBUG, "Using nonciphered write\n");
		res = write(fd, buf, count);
	}
	else
	{
		sleep(1); ssds_log(logDEBUG, "Using ciphered write\n");
		if (_ssl_connected == 0) // not connected
		{
			if (initialize_secure_connection(fd) == -1)
			{
				return ssds_write(fd, buf, count);
				// SSL failed, trying non-SSL
			}
			else
			{
				sleep(1); ssds_log(logMESSAGE, "SSL successfully connected\n");
				_ssl_connected = 1;
			}
		}
		res = SSL_write(ssl, buf, count);
		if (res == -1)
		{
			sleep(1); ssds_log(logERROR, "SSL write failed\n");
			ERR_print_errors_fp(stderr);
		}
	}
	sleep(1); ssds_log(logDEBUG, "Written %d bytes\n", res);
	return res;
}


int initialize_secure_connection(int fd)
{
	sleep(1); ssds_log(logDEBUG, "Init SSL\n");
	sleep(5);
	ctx = InitCTX();

	//server loads certificate and key
	#ifdef __CASE_SERVER
	sleep(1); ssds_log(logDEBUG, "Server is loading certificates\n");
	if (LoadCertificates(ctx, MY_CERT, MY_KEY) == -1)
	{
		sleep(1); ssds_log(logERROR, "SSL fail (server load certs), using non-ciphered connection\n");
		//ERR_print_errors_fp(stderr);
		_use_secure_connection = 0;
		return -1;
	}
	else
	{
		sleep(1); ssds_log(logMESSAGE, "SSL Certificates Successfully loaded\n");
	}
	#endif

	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, fd);

	//client connects to server
	#ifdef __CASE_CLIENT
	if (SSL_connect(ssl) == -1)
	{
		sleep(1); ssds_log(logERROR, "SSL fail (client connect), using non-ciphered connection\n");
		ERR_print_errors_fp(stderr);
		_use_secure_connection = 0;
		return -1;
	}
	else
	{
		sleep(1); ssds_log(logMESSAGE, "Successfully connected\n");
	}
	#endif

	//server accepts client
	#ifdef __CASE_SERVER
	if (SSL_accept(ssl) == -1)
	{
		sleep(1); ssds_log(logERROR, "SSL fail (server accept), using non-ciphered connection\n");
		ERR_print_errors_fp(stderr);
		_use_secure_connection = 0;
		return -1;
	}
	else
	{
		sleep(1); ssds_log(logMESSAGE, "Successfully accepted\n");
	}
	#endif

	if (ShowCerts(ssl) == -1)
	{
		sleep(1); ssds_log(logERROR, "SSL fail (no peer certificates avalaible)\n");
		ERR_print_errors_fp(stderr);
		//_use_secure_connection = 0;
		//return -1;
	}
	return 0;
}


SSL_CTX* InitCTX(void)
{ 
	SSL_METHOD *method;

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	method = SSLv3_client_method();
	ctx = SSL_CTX_new(method);
	if (SSL_CTX_set_cipher_list(ctx, CIPHERS) != 1)
	{
		sleep(1); ssds_log(logERROR, "Couldnt use any ciphers, switching to non-ciphered connection\n");
		_use_secure_connection = 0;
	}
	if ( ctx == NULL )
	{
		ERR_print_errors_fp(stderr);
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
		sleep(1); ssds_log(logDEBUG, "Server certificates:\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		sleep(1); ssds_log(logDEBUG, "Subject: %s\n", line);
		free(line);
		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		sleep(1); ssds_log(logDEBUG, "Issuer: %s\n", line);
		free(line);
		X509_free(cert);
		return 0;
	}
	else
		sleep(1); ssds_log(logERROR, "No peer certificates avalaible.\n");
	return -1;
}


int LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
    {
        //ERR_print_errors_fp(stderr);
        sleep(1); ssds_log(logERROR, "Couldn load certificate\n");
        return -1;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
    {
        //ERR_print_errors_fp(stderr);
        sleep(1); ssds_log(logERROR, "Couldnt load private key\n");
        return -1;
    }
    if (!SSL_CTX_check_private_key(ctx))
    {
        sleep(1); ssds_log(logERROR, "\
        	Private key does not match the certificate public key\n");
        return -1;
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);
    return 0;
}