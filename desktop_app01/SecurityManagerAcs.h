#pragma once

class SecurityManagerAcs
{
public:
    void* getSecureNeworkContext();
    int freeSecureNetworkContext(void* p);
    void setNetworkSd(void* p, int sd);

private:
    void* secureNetworkContext;
};