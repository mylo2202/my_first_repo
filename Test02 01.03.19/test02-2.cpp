#include <bits/stdc++.h>

using namespace std;

long factorial(int x)
{
    cout << "x = " << x << "\n";
    cout << "&x = " << &x << "\n";

    if(x == 0 || x == 1) return 1;
    else return x*factorial(x-1);
}

int main()
{
    int n = 5;

    cout << "factorial(n) = " << factorial(n) << "\n";

    return 0;
}

/*Kết quả:
x = 5
&x = 0x6dfee0
x = 4
&x = 0x6dfec0
x = 3
&x = 0x6dfea0
x = 2
&x = 0x6dfe80
x = 1
&x = 0x6dfe60
factorial(x) = 120
*/

/*Nhận xét:
Trong hàm đệ quy factorial(int x), qua mỗi bước đệ quy giá trị của x giảm đi 1 theo cơ chế tự gọi chính nó của hàm đệ quy.
Địa chỉ bộ nhớ của x qua mỗi bước được xếp vào dưới địa chỉ của x trước đó, liên tiếp và cách đều 32 byte.
Áp dụng cơ chế function call stack, đầu tiên bản sao của đối số n, tức tham số x trong hàm factorial(), được đẩy vào đáy stack.
Sau đó hàm đệ quy thực hiện một bước gọi lại chính nó, tham số mới được gọi ra lại được đẩy vào vị trí trước địa chỉ tham số cũ.
Đến khi hàm đệ quy trở về bước cơ bản và kết thúc gọi tham số thì frame của hàm được xử lí xong.
Stack frame của hàm đệ quy factorial() do đó có kích cỡ bằng 32*N byte, với N là số bước đệ quy hàm thực hiện.
*/
