#ifndef SUM_IMPLIMENT_H_
#define SUM_IMPLIMENT_H_
#include "servant.h"
class Sum : public Servant {
 public:
  int DoRequest(const string &request, vector<char> &buffer);
};
#endif  // SUM_IMPLIMENT_H_