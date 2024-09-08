#include "userinfo.h"

UserInfo &UserInfo::GetInstance()
{
    static UserInfo user;
    return user;
}

UserInfo::UserInfo()
{

}
