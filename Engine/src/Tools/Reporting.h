#pragma once

#include <CkFileAccess.h>
#include <CkRest.h>
#include <CkAuthGoogle.h>

#include <string>

#define JSON_KEY "res/key.json"

#define VALIDATE(result, object)                    \
if (result == false)                                \
{                                                   \
    std::cout << object.lastErrorText() << "\r\n";  \
    return false;                                   \
}     

namespace sixengine
{
    class Reporting
    {
    private:
        CkFileAccess m_FileAcces;
        CkAuthGoogle m_AuthGoogle;
        CkRest m_Rest;

        bool m_IsInitialized;
        std::string m_UserID;

    public:
	    Reporting();

        bool Initialize(std::string tokenPath);
        bool Report(std::string description);
        bool IsInitialized();
    private:
        bool Access();
        bool ValidateUserToken(std::string token);
    };
}