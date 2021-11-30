#include "Tars.h"
#include "TarsType.h"
#include <iostream>
//#include <string>
//#include <algorithm>
#include "Servant.h"
#include "ServantManger.h"
#include "AddImp.h"  
#include "stdio.h"
using namespace std;
using namespace tars;
int main(){


    TarsOutputStream *os=new TarsOutputStream();
    Int32 x=10;
    Int32 y=12;
    os->write(x,1);
    os->write(y,2);

    //创建一个ServantManger
    ServantManager sm;
    
    //添加add服务接口
    sm.addServant<AddImp>("add");

    //收到请求后新建一个add服务接口实例，用这个实例执行用户请求
    ServantPtr p=sm.create("add");

    vector<char> req,res;
    req=os->getByteBuffer();

    //处理req请求，响应记录在res
    p->doRequest(req,res);

    TarsInputStream *is=new TarsInputStream();
    is->setBuffer(res);
    Int32 z;
    is->read(z,1);
    cout<<z<<endl;


    return 0;
}