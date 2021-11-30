#ifndef __TARS_SERVANT_MANAGER_H
#define __TARS_SERVANT_MANAGER_H

#include <iostream>
#include <map>
#include <vector>

#include "Serialization.h"
#include "Servant.h"

class ServantHelperCreation {
 public:
  virtual ServantPtr create(const string &id) = 0;
};
typedef shared_ptr<ServantHelperCreation> ServantHelperCreationPtr;
// typedef ServantHelperCreation* ServantHelperCreationPtr;

template <class T>
struct ServantCreation : public ServantHelperCreation {
  ServantPtr create(const string &id) {
    cout << "ServantPtr create: " << id << endl;
    shared_ptr<T> p = make_shared<T>();
    // T* p =new T();
    p->setName(id);
    return p;
  }
};

class ServantManager {
 public:
  template <typename T>
  void addServant(const string &id) {
    if (servant_creator.find(id) != servant_creator.end()) {
      cerr << "ServantManager::addServant id: " << id << " already exists!"
           << endl;
      throw runtime_error("ServantManager::addServant id: " + id +
                          " already exists!");
    }
    shared_ptr<ServantCreation<T>> p = make_shared<ServantCreation<T>>();
    // ServantCreation<T>* p=new ServantCreation<T>();
    servant_creator[id] = p;
  }

  ServantPtr create(const string &id) {
    ServantPtr servant = NULL;
    if (servant_creator.find(id) != servant_creator.end()) {
      cout << "ServantManager::create servant name " << id << endl;
      servant = servant_creator[id]->create(id);
    }
    return servant;
  }

  int serve(const vector<char> &request, vector<char> &response) {
    InputStream *is = new InputStream();
    is->setBuffer(request);
    String op;
    is->read(op, 0);
    ServantPtr p = create(op);
    if (p != NULL) {
      p->doRequest(request, response);
      return 0;
    } else {
      return -1;
    }
  }

 protected:
  map<string, ServantHelperCreationPtr> servant_creator;

  // TC_ThreadMutex _mutex;
};

#endif
