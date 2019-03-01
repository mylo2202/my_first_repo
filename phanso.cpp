#include <iostream>
using namespace std;

struct Fraction{
    int numerator, denominator;

    Fraction() {}

    Fraction (int _numerator, int _denominator){
        numerator = _numerator;
        denominator = _denominator;
    }
};

int GCD(int a, int b)
{
    while(a!=b)
    {
        if(a>b) a = a-b;
        if(b>a) b = b-a;
    }
    return a;
}

int LCM(int a, int b)
{
    return (a*b)/GCD(a, b);
}

Fraction add(Fraction f1, Fraction f2){
    Fraction sum;

    f1.numerator = f1.numerator*(LCM(f1.denominator, f2.denominator) / f1.denominator);
    f2.numerator = f2.numerator*(LCM(f1.denominator, f2.denominator) / f2.denominator);

    sum.numerator = (f1.numerator + f2.numerator) / GCD((f1.numerator + f2.numerator), LCM(f1.denominator, f2.denominator));
    sum.denominator = LCM(f1.denominator, f2.denominator) / GCD((f1.numerator + f2.numerator), LCM(f1.denominator, f2.denominator));

    return sum;
}

int main() {
    Fraction f1, f2;
    cin >> f1.numerator >> f1.denominator >> f2.numerator >> f2.denominator;

    Fraction sum = add(f1, f2);
    cout << sum.numerator << "/" << sum.denominator;

    return 0;
}

