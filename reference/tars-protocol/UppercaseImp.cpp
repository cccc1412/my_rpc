#include "TarsType.h"
#include "UppercaseImp.h"

#include <algorithm>


using namespace std;

void UppercaseImp::initialize()
{
	cout<<"UppercaseImp::initialize"<<endl;
}


void UppercaseImp::destroy()
{
	cout<<"UppercaseImp::destroy"<<endl;
}

int UppercaseImp::doRequest(const vector<char> &request,  vector<char> &response)
{
	cout<<"UppercaseImp::doRequest"<<endl;
	
	is->setBuffer(request);
	tars::String s;
	is->read(s,1);

	transform(s.begin(), s.end(), s.begin(), ::toupper);  

	os->write(s,1);
	response=os->getByteBuffer();

	return 0;
}
