#include <bits/stdc++.h>

using namespace std;

int main()
{
    int integer;

    int a[3];
    char b[3];

    char character;

    for(int i=0; i<3; i++) cout << "&a[" << i << "] = " << &a[i] << "\n";
    for(int i=0; i<3; i++) cout << "(void*)&b[" << i << "] = "<< (void*)&b[i] << "\n";

    cout << "&integer = " << &integer << "\n";
    cout << "(void*)&character = " << (void*)&character << "\n";

    return 0;
}

/*Kết quả:
&a[0] = 0x6dfee8
&a[1] = 0x6dfeec
&a[2] = 0x6dfef0
(void*)&b[0] = 0x6dfee5
(void*)&b[1] = 0x6dfee6
(void*)&b[2] = 0x6dfee7
&integer = 0x6dfef4
(void*)&character = 0x6dfee4
*/

/*Nhận xét:
Các địa chỉ của các phần tử của mảng a liên tiếp nhau và cách đều nhau 4 byte, nghĩa là 4 byte (32 bit) được dùng để lưu trữ biến int.
Các địa chỉ của các phần tử của mảng b liên tiếp nhau và cách đều nhau 1 byte, nghĩa là 1 byte (8 bit) được dùng để lưu trữ biến char.
Biến integer được khai báo trước mảng được lưu ở địa chỉ bộ nhớ trên mảng.
Biến character được khai báo sau mảng được lưu ở địa chỉ bộ nhớ dưới mảng.
*/
