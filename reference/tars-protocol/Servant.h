#ifndef __TARS_SERVANT_H_
#define __TARS_SERVANT_H_

#include "Tars.h"
#include <memory>

using namespace std;

namespace tars
{

	class Servant
	{
	public:
		Servant()
		{
			is = new TarsInputStream();
			os = new TarsOutputStream();
		}

		~Servant()
		{
			is=NULL;
			os=NULL;
		}

		void setName(const string &name)
		{
			_name = name;
		}

		string getName() const
		{
			return _name;
		}

		virtual void initialize() = 0;

		virtual void destroy() = 0;

		virtual int doRequest(const vector<char> &request,  vector<char> &response) { return -1; }

	protected:

		string _name;

		TarsInputStream *is;
		TarsOutputStream *os;
	};

	typedef shared_ptr<Servant> ServantPtr;
	//typedef Servant* ServantPtr;

}

#endif
