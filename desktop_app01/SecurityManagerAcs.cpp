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
    keymap["client.key"] = std::string();
    keymap["client.crt"] = std::string();
}

void* SecurityManagerAcs::getSecureNeworkContext()
{
    SSL_CTX* ctx;
    SSL* ssl;
    int res = 0;
#ifndef _DEBUG
    if (keymap["client.crt"].size() == 0 || keymap["client.key"].size() == 0) {
        return nullptr;
    }
#endif

    const char* cipher_list = "TLS_AES_128_GCM_SHA256";
    const SSL_METHOD* meth = TLS_client_method();
    ctx = SSL_CTX_new(meth);
    res = SSL_CTX_set_ciphersuites(ctx, cipher_list); //work for TLS 1.3
    //res = SSL_CTX_set_cipher_list(ctx, cipher_list); //only work for TLS 1.2
    res = SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
#ifdef _DEBUG
    res = SSL_CTX_use_certificate_file(ctx, "client.crt", SSL_FILETYPE_PEM);
    res = SSL_CTX_use_PrivateKey_file(ctx, "client.key", SSL_FILETYPE_PEM);
#else
    std::string& client_crt = keymap["client.crt"];
    BIO * bio_x509 = BIO_new_mem_buf(client_crt.c_str(), (int)client_crt.size());
    X509* x509 = PEM_read_bio_X509(bio_x509, NULL, NULL, NULL);
    res = SSL_CTX_use_certificate(ctx, x509);
    X509_free(x509);
    BIO_free(bio_x509);

    std::string& client_key = keymap["client.key"];
    BIO* bio_pkey = BIO_new_mem_buf(client_key.c_str(), (int)client_key.size());
    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio_pkey, NULL, NULL, NULL);
    res = SSL_CTX_use_PrivateKey(ctx, pkey);
    EVP_PKEY_free(pkey);
    BIO_free(bio_pkey);

#endif
    res = SSL_CTX_load_verify_locations(ctx, "rootca.crt", NULL);
    res = SSL_CTX_check_private_key(ctx);
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

bool fileExists(TCHAR* file)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE handle = FindFirstFile(file, &FindFileData);
    bool found = (handle != INVALID_HANDLE_VALUE);
    if (found)
    {
        FindClose(handle);
    }
    return found;
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
                std::string out;
                TCHAR szPath[MAX_PATH];

                _stprintf_s(szPath, _T("%s\\cert\\client.key"), szRootpath);
                wstr.assign((wchar_t*)szPath);
                for (wchar_t ch : wstr) {
                    out.push_back(static_cast<char>(ch));
                }
                std::string clientkey = readFile(out);
                out.clear();

                _stprintf_s(szPath, _T("%s\\cert\\client.crt"), szRootpath);
                wstr.assign(szPath);
                for (wchar_t ch : wstr) {
                    out.push_back(static_cast<char>(ch));
                }
                std::string clientcrt = readFile(out);
                out.clear();
                bool a = false, b = false;

                if (clientkey.size() != 0) { keymap["client.key"] = clientkey; a = true; }
                if (clientcrt.size() != 0) { keymap["client.crt"] = clientcrt; b = true; }
                if (a && b) {
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
    BIO* bio_sign;
    CMS_ContentInfo* cms;
    BIO* bio_content;
    BIO* bio_ca;
    X509* x509_ca;
    int ret = -1;

    auto cms_der_in = reinterpret_cast<const unsigned char*>(sign.c_str());
    long cms_der_length = sign.size();

    cms = d2i_CMS_ContentInfo(NULL, &cms_der_in, cms_der_length);

    bio_content = BIO_new_mem_buf(content.c_str(), content.size());
    bio_ca = BIO_new_mem_buf(rootca.c_str(), rootca.size());
    x509_ca = PEM_read_bio_X509(bio_ca, NULL, NULL, NULL);

    X509_STORE* store = X509_STORE_new();
    X509_STORE_add_cert(store, x509_ca);

    int flags = CMS_BINARY;
    if (CMS_verify(cms, NULL, store, bio_content, NULL, flags) == 1) {
        ret = 1;
    }

    X509_STORE_free(store);
    X509_free(x509_ca);
    BIO_free(bio_ca);
    BIO_free(bio_content);
    CMS_ContentInfo_free(cms);
    //BIO_free(bio_sign);

    return ret;
}

bool SecurityManagerAcs::readConfig(std::string& ip, std::string& port, std::string& secureport)
{
    std::string sign = readFile("clientconf.sign");
    std::string content = readFile("clientconf.bin");
    std::string rootca = readFile("rootca.crt");

    if (1 != cmsVerify(sign, content, rootca)) {
        return false;
    }

    std::istringstream iss(content);
    std::getline(iss, ip);
    std::getline(iss, port);
    std::getline(iss, secureport);

    return true;
}
