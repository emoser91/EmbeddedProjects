/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
// Online Compiler: https://www.onlinegdb.com/online_c++_compiler

#include <iostream>
#include <string.h> //lets us do string manipulation

using namespace std;

//typedef: creates a new name for an existing thing
typedef int my_int; //can use my_int for int now

#define OFF 0
#define ON 1

int gvar = 100; //global varriable 
const float pi = 3.14159;

////////////////////////////////////////
// Loops and Switch
////////////////////////////////////////

int main()
{
    //Data Types
    short int age = 25;
    float average; //uninitiated will have garbage value
    char gender = 'm';
    bool isit = true;
    average = 22.52;
    
    cout << age << endl;
    cout << average << endl;
    cout << gender << endl;
    cout << isit << endl;
    
    float variable = 5.5; //will default to a double!
    float var = 5.5f; //will force to float
    double varb = 5.5f;
    
    
    //Strings in c and using string.h
    char strex1[6] = {'H', 'e', 'l', 'l', 'o', '\0'};
    char strex2[] = "Hello";
    
    char str1[12] = "Hello";
    char str2[12] = "World";
    char str3[12];
    int  len ;

    /* copy str1 into str3 */
    strcpy(str3, str1);
    printf("strcpy( str3, str1) :  %s\n", str3 );

    /* concatenates str1 and str2 */
    strcat( str1, str2);
    printf("strcat( str1, str2):   %s\n", str1 );

    /* total lenghth of str1 after concatenation */
    len = strlen(str1);
    printf("strlen(str1) :  %d\n", len );
   

    return 0;
}