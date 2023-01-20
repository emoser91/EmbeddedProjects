/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
// Online Compiler: https://www.onlinegdb.com/online_c++_compiler

#include <iostream>

using namespace std;

////////////////////////////////////////
// Arrays
////////////////////////////////////////

//Print array using pointers
void print_array(string *array, int length)
{
    for(int i = 0; i < length; i++)
    {
        cout << array[i] << ' ';
    }
}

//Print array
void print_array2(string array[], int length)
{
    for(int i = 0; i < length; i++)
    {
        cout << array[i] << ' ';
    }
}

//Print array passing start and end pointers
void print_array3(string *start, string *end)
{
    for(string *i = start; i != end; i++)
    {
        cout << *i << ' ';
    }
}

int main()
{
    cout << "Hello World" << endl;
    
    string arrayCars[4] = {"BMW", "AUDI", "Ford", "Mazda"}; //or int array[] = {0, 1, 2, 3};
    
    //You can use either of these inside the functions as well
    arrayCars[0] = "Porsche"; //Change value at spot 0 to "Porsche"
    *(arrayCars + 2) = "Lambo"; //Changing a value with pointers at position 2
    
    int length = sizeof(arrayCars)/sizeof(arrayCars[0]);
    
    //Basic Printing Array
    for(int i = 0; i < length; i++)
    {
        cout << arrayCars[i] << ' ';
    }
    
    //Function that Prints Array
    print_array(arrayCars, length);
    
    print_array2(arrayCars, length);
    
    print_array3(arrayCars, arrayCars + length);
    
    //2D Arrays
    int array2d[2][6] = { {10, 20, 30, 40, 50, 60}, {11, 22, 33, 44, 55, 66} };
    int getarray = array2d[0][2]; //sets to value 30
    cout << getarray << endl;

    return 0;
}
