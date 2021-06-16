#include "SecurityManagerAcs.h"

#include <WinSock2.h>
#include "openssl/ssl.h"

void* SecurityManagerAcs::getSecureNeworkContext()
{
    SSL_CTX* ctx;
    SSL* ssl;
    int res = 0;

    const char* cipher_list = "TLS_AES_128_GCM_SHA256";
    const SSL_METHOD* meth = TLS_client_method();
    ctx = SSL_CTX_new(meth);
    res = SSL_CTX_set_ciphersuites(ctx, cipher_list); //work for TLS 1.3
    //res = SSL_CTX_set_cipher_list(ctx, cipher_list); //only work for TLS 1.2
    res = SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
    res = SSL_CTX_use_certificate_file(ctx, "client.crt", SSL_FILETYPE_PEM);
    res = SSL_CTX_use_PrivateKey_file(ctx, "client.key", SSL_FILETYPE_PEM);
    res = SSL_CTX_check_private_key(ctx);
    res = SSL_CTX_load_verify_locations(ctx, "rootca.crt", NULL);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); // mtls, fail if no cert of client
    ssl = SSL_new(ctx);

    secureNetworkContext = reinterpret_cast<void*>(ctx);
    return (void*) ssl;
}

int SecurityManagerAcs::freeSecureNetworkContext(void* p)
{
    SSL_free((SSL*)p);
    SSL_CTX_free((SSL_CTX*)secureNetworkContext);
    return 0;
}

void SecurityManagerAcs::setNetworkSd(void* p, int sd)
{
    SSL* ssl = reinterpret_cast<SSL*>(p);
    SSL_set_fd(ssl, sd);
    SSL_connect(ssl);
}
