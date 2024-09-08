#ifndef USERINFO_H
#define USERINFO_H
#include <QString>

class UserInfo
{
public:
    static UserInfo& GetInstance();
    UserInfo(const UserInfo&) = delete;
    UserInfo& operator=(const UserInfo&) = delete;

    inline void setId(QString id){ id_=id; }
    inline void setToken(QString token) { token_ = token;}
    inline QString id(){ return id_; }
    inline QString token(){ return token_; }
private:
    QString id_;
    QString token_;
    UserInfo();
};

#endif // USERINFO_H
