#include "status.h"
Status::Status(long l_return_code, long l_error_code):cl_return_code_(l_return_code_), cl_error_code_(l_return_code_) {
    l_return_code_ = l_return_code;
    l_error_code_ = l_return_code;
}

Status::~Status() {
}

Status::Status(const Status& s):cl_return_code_(l_return_code_), cl_error_code_(l_error_code_) {
    l_return_code_ = s.l_return_code_;
    l_error_code_ = s.l_error_code_;
}

bool Status::is_success() {
    if(cl_return_code_ >= 0)
        return true;
    else
        return false;
}

