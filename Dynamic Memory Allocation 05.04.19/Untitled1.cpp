#include <iostream>
#include <cstring>

using namespace std;

const char stringA[] = "Hello";
const char stringB[] = "World";

char* comcat(const char* stringA, const char* stringB)
{
    int stringResultSize = strlen(stringA) + strlen(stringB);
    char* stringResult = new char[stringResultSize];
    strcpy(stringResult, stringA);
    strcat(stringResult, stringB);
    return stringResult;

}

int main()
{
    cout << comcat(stringA, stringB);

    return 0;
}
