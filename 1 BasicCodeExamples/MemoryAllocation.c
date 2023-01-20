/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
// Online Compiler: https://www.onlinegdb.com/online_c++_compiler

#include <iostream>

using namespace std;

////////////////////////////////////////
// Dynamic Memory Allocation for C++
////////////////////////////////////////
/* "new" keyword is used to allocate memory from the heap and return its address
   "delete" keyword is used to deallocate memory
   
   NOTE:
   malloc(): It is a C library function that can also be used in C++, while the “new” 
   operator is specific for C++ only. Both malloc() and new are used to allocate the 
   memory dynamically in heap. But “new” does call the constructor of a class 
   whereas “malloc()” does not.
   
   The delete operator is used to delete the pointer, which is either allocated using 
   "new" operator or a NULL pointer, whereas the free() function is used to delete the 
   pointer that is either allocated using malloc(), calloc() or realloc() function or NULL pointer
*/

// C++ Example
int main()
{
    //Basic example of memory allocation
    int *pointer;
    pointer = new int; //will allocate heap memory to store int
    cout << pointer << endl; //pointer address
    *pointer = 24; //storing value of 24
    
    cout << *pointer << endl; //prints value of 24
    
    delete pointer; //deallocate memory
    
    //Dynamically allocating arrays
    int *ptrarray = new int[10]; //allocates array size 10
    ptrarray[0] = 12;
    cout << ptrarray[0] << endl;
    delete[] ptrarray; //deallocates memory
    
    //Avoiding Dangling Pointer Reference
    //pointer that refers to a once allocated memory location but has been deleted
    int *ptr = nullptr; //keyword to get pointers to NULL
    ptr = new int;
    if(ptr != nullptr)
    {
        *ptr = 10;
        cout << *ptr << endl;
        delete ptr;
        ptr = nullptr;
    }
    else
    {
        cout << "Memory not allocated" << endl;
    }
    
    return 0;
}

//C Example (can also be used in C++)
int main()
{
	//Dynamically allocate array of length user enters
	int *a;
	int length = 0;
	
	printf("Enter a length: ");
	scanf("%d", &length);
	
	//NOTE: if you are using a C++ compiler, you NEED to cast in the front of malloc to the data type of the pointer!
	//If you are just using a C compiler then you do not need to cast at all. 
	a = (int*)malloc(length * sizeof(int)); //dynamically allocating memory. The malloc() function takes length as argument
	
	//Filling Array
	for (int i = 0; i < length; i++)
	{
		a[i] = i;
	}
	
	//Printing Array
	for (int i = 0; i < length; i++)
	{
		printf("a[%d]=%d\n", i, a[i]);
	}
	
	free(a); //deallocates memory
	
	return 0;
}