#include <pch.h>

#include "Reporting.h"

#include <iostream>
#include <CkSocket.h>
#include <CkJsonObject.h>
#include <chrono>


sixengine::Reporting::Reporting()
{
    
}

bool sixengine::Reporting::Initialize(std::string tokenPath)
{
    m_UserID = "";
    m_IsInitialized = false;

    auto id = m_FileAcces.readEntireTextFile(tokenPath.c_str(), "utf-8");

    if (m_FileAcces.get_LastMethodSuccess() == false)
    {
        std::cout << m_FileAcces.lastErrorText() << std::endl;
        return false;
    }

    bool accessed = Access();
    bool userValidated = ValidateUserToken(id);

    if (accessed && userValidated)
    {
        m_UserID = id;
        m_IsInitialized = true;

        return true;
    }

    return false;
}

bool sixengine::Reporting::Report(std::string description)
{
    if (m_IsInitialized == false)
    {
        return false;
    }

    if (description.size() == 0)
    {
        return false;
    }

    CkJsonObject issueRecord;

    issueRecord.AddObjectAt(0, "comments");
    issueRecord.ObjectAt(0)->AppendInt("count", 0);

    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    std::string date = std::to_string(ms.count());

    issueRecord.AppendString("content", description.c_str());
    issueRecord.AddNumberAt(-1, "date", date.c_str());
    issueRecord.AppendString("type", "ISSUE");
    issueRecord.AppendString("userid", this->m_UserID.c_str());

    std::cout << "SUCCESS" << std::endl;

    const char* jsonResponse = this->m_Rest.fullRequestString("POST", "/posts.json", issueRecord.emit());

    VALIDATE(m_Rest.get_LastMethodSuccess(), m_Rest);
    VALIDATE((m_Rest.get_ResponseStatusCode() == 200), m_Rest);

    return true;
}

bool sixengine::Reporting::IsInitialized()
{
    return m_IsInitialized;
}

bool sixengine::Reporting::Access()
{
    const char* jsonKey = m_FileAcces.readEntireTextFile(JSON_KEY, "utf-8");
    VALIDATE(m_FileAcces.get_LastMethodSuccess(), m_FileAcces);
    
    m_AuthGoogle.put_JsonKey(jsonKey);

    m_AuthGoogle.put_Scope("https://www.googleapis.com/auth/firebase.database https://www.googleapis.com/auth/userinfo.email");
    m_AuthGoogle.put_ExpireNumSeconds(3600);
    m_AuthGoogle.put_SubEmailAddress("");

    CkSocket tlsSocket;
    bool success = tlsSocket.Connect("www.googleapis.com", 443, true, 5000);
    VALIDATE(success, tlsSocket);

    VALIDATE(m_AuthGoogle.ObtainAccessToken(tlsSocket), m_AuthGoogle);

    return true;
}

bool sixengine::Reporting::ValidateUserToken(std::string token)
{
    bool success = this->m_Rest.Connect("notdune-website.firebaseio.com", 443, true, true);
    VALIDATE(success, m_Rest);

    this->m_Rest.SetAuthGoogle(m_AuthGoogle);
    
    std::string path = "/users/" + token;
    const char* jsonResponse = this->m_Rest.fullRequestNoBody("GET", path.c_str());
    VALIDATE(m_Rest.get_LastMethodSuccess(), m_Rest);

    return (jsonResponse != nullptr);
}
