#include <bits/stdc++.h>

using namespace std;

int main()
{
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    int n, m;
    cin >> n;
    int arr[10000] = {0};

    for (int i = 0; i < n; i++)
    {
        int x;
        cin >> x;
        arr[x]--;
    }
    cin >> m;
    for (int i = 0; i < m; i++)
    {
        int x;
        cin >> x;
        arr[x]++;
    }
    for (int i = 0; i < 10000; i++)
    {
        if (arr[i] != 0)
        cout << i << " ";
    }

    return 0;
}
