#ifndef SERVANT_H_
#define SERVANT_H_

#include <memory>

#include "Serialization.h"

using namespace std;

class Servant {
 public:
  Servant() {
    is = new InputStream();
    os = new OutputStream();
  }

  ~Servant() {
    is = NULL;
    os = NULL;
  }

  void setName(const string &name) { _name = name; }

  string getName() const { return _name; }

  virtual void initialize() = 0;

  virtual void destroy() = 0;

  virtual int doRequest(const vector<char> &request, vector<char> &response) {
    return -1;
  }

 protected:
  string _name;

  InputStream *is;
  OutputStream *os;
};

typedef shared_ptr<Servant> ServantPtr;
// typedef Servant* ServantPtr;

#endif
