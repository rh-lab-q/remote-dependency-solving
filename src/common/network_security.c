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


ssize_t ssds_read(int fd, void *buf, size_t count)
{
	int res = -1;
	if (_use_secure_connection == 0)
	{
		res = read(fd, buf, count);
	}
	else
	{
		if (_ssl_connected == 0) // initialize connection
		{
			if (initialize_secure_connection(fd) == -1)
				return 0;
			else
				_ssl_connected = 1;
		}
		res = SSL_read(ssl, buf, count);
		if (res == -1)
		{
			printf("SSL read failed\n");
			ERR_print_errors_fp(stderr);
		}
	}
	return res;
}


ssize_t ssds_write(int fd, const void *buf, size_t count)
{
	int res = -1;
	if (_use_secure_connection == 0)
	{
		res = write(fd, buf, count);
	}
	else
	{
		if (_ssl_connected == 0) // not connected
		{
			if (initialize_secure_connection(fd) == -1)
				return 0;
			else
				_ssl_connected = 1;
		}
		res = SSL_write(ssl, buf, count);
		if (res == -1)
		{
			printf("SSL write failed\n");
			ERR_print_errors_fp(stderr);
		}
	}
	return res;
}


int initialize_secure_connection(int fd)
{
	ctx = InitCTX();
	
	//server loads certificate and key
	#ifdef _SERVER_H
	LoadCertificates(ctx, MY_CERT, MY_KEY);
	#endif
	
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, fd);

	//client connects to server
	#ifdef _CLIENT_H
	if (SSL_connect(ssl) == -1)
	{
		printf("SSL fail, using non-ciphered connection\n");
		ERR_print_errors_fp(stderr);
		_use_secure_connection = 0;
		return -1;
	}
	#endif

	//server accepts client
	#ifdef _SERVER_H
	if (SSL_accept(ssl) == -1)
	{
		printf("SSL fail, using non-ciphered connection\n");
		ERR_print_errors_fp(stderr);
		_use_secure_connection = 0;
		return -1;
	}
	#endif

	ShowCerts(ssl);
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
		printf("couldnt use any ciphers, switching to non-ciphered connection\n");
		_use_secure_connection = 0;
	}    
	if ( ctx == NULL )
	{
		ERR_print_errors_fp(stderr);
	}
	return ctx;
}


void ShowCerts(SSL* ssl)
{ 
	X509 *cert;
	char *line;
	cert = SSL_get_peer_certificate(ssl);
	if ( cert != NULL )
	{
		printf("Server certificates:\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		printf("Subject: %s\n", line);
		free(line);
		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		printf("Issuer: %s\n", line);
		free(line);
		X509_free(cert);
	}
	else
		printf("No certificates.\n");
}