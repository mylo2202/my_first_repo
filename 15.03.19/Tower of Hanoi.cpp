#include <bits/stdc++.h>

using namespace std;

void move(int n, char from, char to, char auxiliary)
{
    if (n == 1)
    {
        cout << "Move disc 1 from peg " << from << " to peg " << to << "\n";
    }
    else
    {
        move(n-1, from, auxiliary, to);
        cout << "Move disc " << n << " from peg " << from << " to peg " << to << "\n";
        move(n-1, auxiliary, to, from);
    }
}

int main()
{
    int n;
    cin >> n;
    move(n, 'A', 'C', 'B');
    return 0;
}
