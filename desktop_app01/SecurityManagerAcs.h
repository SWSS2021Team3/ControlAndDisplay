#pragma once
#include <unordered_map>

class SecurityManagerAcs
{
public:
    SecurityManagerAcs();
    void* getSecureNeworkContext();
    int freeSecureNetworkContext(void* p);
    void setNetworkSd(void* p, int sd);
    bool readKey();

private:
    std::unordered_map<std::string, std::string> keymap;
    void* secureNetworkContext;
};