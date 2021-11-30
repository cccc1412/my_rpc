#ifndef _UppercaseImp_H_
#define _UppercaseImp_H_

#include "Servant.h"

class UppercaseImp : public Servant {
 public:
  virtual void initialize();

  virtual void destroy();

  virtual int doRequest(const vector<char> &request, vector<char> &response);
};

#endif
