#ifndef SERVANT_H_
#define SERVANT_H_

#include "epoller_server.h"
#include <vector>
//编写新的业务逻辑，需要继承Servant，然后实现DoRequest
class Servant {
public:
    Servant();
    ~Servant();
    void SetHandle(Handle* handle);
    Handle* GetHandle();
    string GetName() const;
    void SetName(const string &name);
    virtual int Dispatch(const string &request, vector<char>&buffer);
    virtual int DoRequest(const string &request, vector<char>&buffer) {return -1;};//解析request，返回结果放在buffer里

protected:
    Handle* handle_;
    string name_;
};

#endif //SERVANT_H_