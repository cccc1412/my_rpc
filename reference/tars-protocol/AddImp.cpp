#include "TarsType.h"
#include "AddImp.h"
#include <stdio.h>
using namespace std;

void AddImp::initialize()
{
	cout<<"AddImp::initialize"<<endl;
}


void AddImp::destroy()
{
	cout<<"AddImp::destroy"<<endl;
}

int AddImp::doRequest(const vector<char> &request,  vector<char> &response)
{
	cout<<"AddImp::doRequest"<<endl;

	is->setBuffer(request);
	tars::Int32 i1,i2;
	is->read(i1,1);
	is->read(i2,2);

	tars::Int32 i=i1+i2;
	
	os->write(i,1);
	response=os->getByteBuffer();

	return 0;
}
