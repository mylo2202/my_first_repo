#include <bits/stdc++.h>

using namespace std;

int main()
{
    int testcases, n;
    cin >> testcases;

    int i, query;
    for(query = 1; query <= testcases; query++)
    {
        cin >> n;
        int j, leftSum = 0, rightSum = 0;
        bool isBalancedSum = false;
        long long int arr[n];
        for(i = 0; i < n; i++)
        {
            cin >> arr[i];
            rightSum += arr[i];
        }
        for(i = 0; i < n; i++)
        {
            if(i >= 1)
            leftSum += arr[i-1];
            rightSum -= arr[i];
            if(leftSum == rightSum)
            {
                isBalancedSum = true;
                break;
            }
        }
        if(isBalancedSum == true)
            cout << "YES" << "\n";
        else if(isBalancedSum == false || n == 1)
            cout << "NO" << "\n";
    }

    return 0;
}
