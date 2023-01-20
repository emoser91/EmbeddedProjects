/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
// Online Compiler: https://www.onlinegdb.com/online_c++_compiler

#include <iostream>

using namespace std;

////////////////////////////////////////
// Pointers Basics
////////////////////////////////////////
/* A variable we create has a name and we set a value for it that is stored in RAM
   but that variable also has a memory address.
   We can make a variable called a pointer which stores the address of the variable we created. 
   
   We use pointers for creating pass by reference/ pass by pointer so we can make a function that can
   modify multiple variable instead of just giving a return. The second reason is for dynamic memory allocation.
*/ 

int main()
{
	int b = 42;
	
	int *a = &b; //We are creating a int type pointer named "a" the has the value equal to the address
				 //of variable "b". Thus, a pointer to b memory address.
				 //The "&" operator returns the memory address of a variable
				 
	printf("b: %d\n", b);
	//The following prints will be the same value
	printf("&b: %p\n", &b); //%p is used to print a pointer
	printf("a: %p\n", a);
	cout << "&b: " << &b << endl; //cout example
	
	
	// Changing variable values using a pointer
	*a = 50; //You can use the * operator to dereference a pointer variable to get the value at the address
	
	//We changed the value of b using our "a"pointer to b memory address but the memory address will stay the same
	printf("b: %d\n", b);
	//The following prints will be the same value
	printf("&b: %p\n", &b); //%p is used to print a pointer
	printf("a: %p\n", a);
	
	
	// Pass by reference example
	int x, y, z;
	x = y = z = 0;
	
	printf("Enter 3 Numbers: ");
	scanf("%d %d %d", &x, &y, &z); //Scanf uses pass by reference to take in the variable and then modify them
	printf("Result: %d\n", x + y + z);
	
	return 0;
}






////////////////////////////////////////
// Pointers Swap Numbers
////////////////////////////////////////
//Swapnum takes two variables that contain memory addresses
//We use pass by reference so we can change both variables values
//If we didnt use pointers, we can only give a single return out of the function
//We could swap without pointers inside of main but not in an external function
void swapnum(int *num1, int *num2)
{
   int tempnum;

   tempnum = *num1;
   *num1 = *num2;
   *num2 = tempnum;
}

int main()
{
    cout << "Hello World" << endl;
    
    int age = 24;
    bool human = true;
    
    int *ageptr; //declare a pointer
    bool *humanptr;
    
    ageptr = &age; //set pointer to the memory address of age
    humanptr = &human;
    
    *ageptr = 25; //changes the value at memory address to 25 (it was 24)
    
    //double pointer
    int **dptrage = &ageptr;
    
    //Example of swapping two numbers with Pointers
    int v1 = 11, v2 = 77 ;
    printf("Before swapping:");
    printf("\nValue of v1 is: %d", v1);
    printf("\nValue of v2 is: %d", v2);

    //calling swap function
    swapnum( &v1, &v2 ); //could also make a int *v1ptr = &v1 and just pass v1ptr and do the same for v2

    printf("\nAfter swapping:");
    printf("\nValue of v1 is: %d", v1);
    printf("\nValue of v2 is: %d", v2);

    return 0;
}
