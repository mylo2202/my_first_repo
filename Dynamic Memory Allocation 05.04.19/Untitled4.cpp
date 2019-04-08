//09.DynamicMemory A4

#include <iostream>
using namespace std;

int main()
{
    int x = 0;
    int *p = new int;
    p = &x;
    delete p;
    cout << p << '\n';
    cout << *p << '\n';
    cout << x << '\n';

    /*
    khi giải phóng vùng bộ nhớ p trỏ tới không xác định được con trỏ p sau đó trỏ tới đâu
    và khả năng rất cao là nó trỏ tới vùng bộ nhớ không tồn tại, dẫn đến chương trình sập
    */

    return 0;
}
