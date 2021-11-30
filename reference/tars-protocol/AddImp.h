#ifndef _AddImp_H_
#define _AddImp_H_

#include "Servant.h"
using namespace tars;
class AddImp : public Servant
{
public:
	AddImp():Servant(){};
	~AddImp(){};

    virtual void initialize();

	virtual void destroy();

	virtual int doRequest(const vector<char> &request,  vector<char> &response);
};

#endif
