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
    bool readConfig(std::string& ip, std::string& port, std::string& secureport);

private:
    std::unordered_map<std::string, std::string> keymap;
    void* secureNetworkContext;
};