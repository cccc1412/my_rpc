#include "SumImp.h"

#include <stdio.h>

#include "Type.h"
using namespace std;

void SumImp::initialize() { cout << "SumImp::initialize" << endl; }

void SumImp::destroy() { cout << "SumImp::destroy" << endl; }

int SumImp::doRequest(const vector<char> &request, vector<char> &response) {
  cout << "SumImp::doRequest" << endl;

  is->setBuffer(request);
  Float i1, i2;
  is->read(i1, 1);
  is->read(i2, 2);

  Float i = i1 + i2;

  os->write(i, 1);
  response = os->getByteBuffer();

  return 0;
}
