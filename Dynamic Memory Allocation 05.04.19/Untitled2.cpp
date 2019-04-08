//09.DynamicMemory A2

#include <bits/stdc++.h>
using namespace std;

int main()
{
    int* p = new int;
    int* p2 = p;
    *p = 10;
    //delete p; //giai phong bo nho qua som
    //*p2 = 100;
    cout << *p2;
    delete p;
    delete p2;


    return 0;
}
