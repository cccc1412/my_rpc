#ifndef _SumImp_H_
#define _SumImp_H_

#include "Servant.h"

class SumImp : public Servant {
 public:
  SumImp() : Servant(){};
  ~SumImp(){};

  virtual void initialize();

  virtual void destroy();

  virtual int doRequest(const vector<char> &request, vector<char> &response);
};

#endif
