#ifndef STATUS_H_
#define STATUS_H_
class Status {
public:
    Status(long l_return_code, long l_error_code);//return code >= 0 表示成功，否则失败
    Status(const Status& s);
    virtual ~Status();

public:
    bool is_success();

public:
    const long& cl_return_code_;
    const long& cl_error_code_;

private:
    long l_return_code_;
    long l_error_code_;
};

#endif //STATUS_H_