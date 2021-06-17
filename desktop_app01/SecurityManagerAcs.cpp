#include "SecurityManagerAcs.h"

#include <WinSock2.h>
#include <tchar.h>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "openssl/ssl.h"
#include "openssl/cms.h"

SecurityManagerAcs::SecurityManagerAcs() : secureNetworkContext(nullptr)
{
}

void* SecurityManagerAcs::getSecureNeworkContext()
{
    SSL_CTX* ctx;
    SSL* ssl;
    int res = 0;

    if (keymap.find("client.crt") == keymap.end() 
        || keymap.find("client.key") == keymap.end()
        || keymap.find("rootca.crt") == keymap.end()) {
        return nullptr;
    }

    const char* cipher_list = "TLS_AES_128_GCM_SHA256";
    const SSL_METHOD* meth = TLS_client_method();
    ctx = SSL_CTX_new(meth);
    if (ctx == NULL) {
        return nullptr;
    }
    if (1 != SSL_CTX_set_ciphersuites(ctx, cipher_list)) { //SSL_CTX_set_cipher_list for TLS 1.2
        goto error_exit;
    }
    if (1 != SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION)) {
        goto error_exit;
    }
    if (1 != SSL_CTX_use_certificate_file(ctx, keymap["client.crt"].c_str(), SSL_FILETYPE_PEM)) {
        goto error_exit;
    }
    if (1 != SSL_CTX_use_PrivateKey_file(ctx, keymap["client.key"].c_str(), SSL_FILETYPE_PEM)) {
        goto error_exit;
    }
    if (1 != SSL_CTX_load_verify_locations(ctx, keymap["rootca.crt"].c_str(), NULL)) {
        goto error_exit;
    }
    if (1 != SSL_CTX_check_private_key(ctx)) {
        goto error_exit;
    }
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); // mtls, fail if no cert of client
    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        goto error_exit;
    }
    secureNetworkContext = reinterpret_cast<void*>(ctx);
    return (void*) ssl;
error_exit:
    SSL_CTX_free(ctx);
    return nullptr;
}

int SecurityManagerAcs::freeSecureNetworkContext(void* p)
{
    if (p != nullptr) {
        SSL_free((SSL*)p);
        SSL_CTX_free((SSL_CTX*)secureNetworkContext);
    }
    return 0;
}

int SecurityManagerAcs::setNetworkSd(void* p, int sd)
{
    if (p != nullptr) {
        SSL* ssl = reinterpret_cast<SSL*>(p);
        SSL_set_fd(ssl, sd);
        int ret = SSL_connect(ssl);
        if (ret <= 0) {
            return -1;
        }
    }
    return 1;
}

std::string readFile(const std::string& path)
{
    std::string result;
    FILE* fp = nullptr;
    auto err = fopen_s(&fp, path.c_str(), "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        result.resize((std::max)(ftell(fp), (long int)0));
        fseek(fp, 0, SEEK_SET);
        auto len = fread(&(result[0]), 1, result.size(), fp);
        fclose(fp);
        if (len != result.size()) {
            result.resize(len);
        }
    }
    return result;
}

bool SecurityManagerAcs::readKey()
{
    bool ret = false;
    DWORD dwDrives = GetLogicalDrives();
    if (0 == dwDrives) return ret;

    TCHAR chDriveLabel = 'A';
    TCHAR szRootpath[MAX_PATH];
    memset(szRootpath, 0, sizeof(TCHAR) * MAX_PATH);
    while (dwDrives != 0) {
        if ((dwDrives & 1) != 0) {
            _stprintf_s(szRootpath, _T("%c:\\"), chDriveLabel);
            if (DRIVE_REMOVABLE == GetDriveType(szRootpath)) {
                std::wstring wstr;
                std::string clientkeypath, clientcrtpath, rootcacrtpath;
                TCHAR szPath[MAX_PATH];

                _stprintf_s(szPath, _T("%s\\cert\\client.key"), szRootpath);
                wstr.assign((wchar_t*)szPath);
                for (wchar_t ch : wstr) {
                    clientkeypath.push_back(static_cast<char>(ch));
                }
                std::string clientkey = readFile(clientkeypath);

                _stprintf_s(szPath, _T("%s\\cert\\client.crt"), szRootpath);
                wstr.assign(szPath);
                for (wchar_t ch : wstr) {
                    clientcrtpath.push_back(static_cast<char>(ch));
                }
                std::string clientcrt = readFile(clientcrtpath);

                _stprintf_s(szPath, _T("%s\\cert\\rootca.crt"), szRootpath);
                wstr.assign(szPath);
                for (wchar_t ch : wstr) {
                    rootcacrtpath.push_back(static_cast<char>(ch));
                }
                std::string rootcacrt = readFile(rootcacrtpath);

                if (clientkey.size() != 0 && clientcrt.size() && rootcacrt.size()) {
                    keymap["client.key"] = clientkeypath;
                    keymap["client.crt"] = clientcrtpath;
                    keymap["rootca.crt"] = rootcacrtpath;
                    ret = true;
                    break;
                }
            }
        }
        dwDrives = dwDrives >> 1;//next drive
        chDriveLabel++;
    }

    return ret;
}

int cmsVerify(std::string& sign, std::string& content, std::string& rootca) {
    CMS_ContentInfo* cms = NULL;
    BIO* bio_content = NULL;
    BIO* bio_ca = NULL;
    X509* x509_ca = NULL;
    X509_STORE* store = NULL;
    int ret = -1;
    int flags = CMS_BINARY;
    auto cms_der_in = reinterpret_cast<const unsigned char*>(sign.c_str());
    long cms_der_length = (long)sign.size();

    cms = d2i_CMS_ContentInfo(NULL, &cms_der_in, cms_der_length);
    if (cms == NULL) {
        ret -2;
        goto error_exit;
    }
    bio_content = BIO_new_mem_buf(content.c_str(), (int)content.size());
    if (bio_content == NULL) {
        ret = -3;
        goto error_exit;
    }
    bio_ca = BIO_new_mem_buf(rootca.c_str(), (int)rootca.size());
    if (bio_ca == NULL) {
        ret = -4;
        goto error_exit;
    }
    x509_ca = PEM_read_bio_X509(bio_ca, NULL, NULL, NULL);
    if (x509_ca == NULL) {
        ret = -5;
        goto error_exit;
    }
    store = X509_STORE_new();
    if (store == NULL) {
        ret = -6;
        goto error_exit;
    }
    X509_STORE_add_cert(store, x509_ca);

    if (CMS_verify(cms, NULL, store, bio_content, NULL, flags) == 1) {
        ret = 1;
    }

error_exit:
    if (store != NULL) X509_STORE_free(store);
    if (x509_ca != NULL) X509_free(x509_ca);
    if (bio_ca != NULL) BIO_free(bio_ca);
    if (bio_content != NULL) BIO_free(bio_content);
    if (cms != NULL) CMS_ContentInfo_free(cms);

    return ret;
}

bool SecurityManagerAcs::readConfig(std::string& ip, std::string& port, std::string& secureport)
{
    std::string sign = readFile("clientconf.sign");
    std::string content = readFile("clientconf.bin");
    std::string rootca = readFile(keymap["rootca.crt"]);

    if (1 != cmsVerify(sign, content, rootca)) {
        return false;
    }

    std::istringstream iss(content);
    std::getline(iss, ip);
    std::getline(iss, port);
    std::getline(iss, secureport);

    return true;
}
