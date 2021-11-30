#include <vector>
#include <iostream>
using namespace std;

int main(){
    int a[3]={1,2,3};
    int *x=a;
    vector<int> y;
    y.assign(x,x+3);
    y[0]=4;
    cout<<y[1]<<a[0]<<endl;

}