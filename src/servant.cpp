#include "servant.h"

Servant::Servant() {

}

Servant::~Servant() {

}

void Servant::SetName(const string &name) {
    name_ = name;
}

string Servant::GetName() const {
    return name_;
}

void Servant::SetHandle(Handle *handle) {
    handle_ = handle;
}

Handle* Servant::GetHandle() {
    return handle_;
}

int Servant::Dispatch(const string &request, vector<char>&buffer) {
    int ret = DoRequest(request, buffer);
    return ret;
}