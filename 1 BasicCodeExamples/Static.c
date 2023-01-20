/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
// Online Compiler: https://www.onlinegdb.com/online_c++_compiler

#include <iostream>

using namespace std;

////////////////////////////////////////
// Classes C++ Static Example
////////////////////////////////////////
//Note: You can not declare a static class in C++
class Person 
{
    //Public: can be accessed by everyone
    public:
    
        //Static varriable: space gets allocated for the lifetime of the program
        //NOTE: You cant initialize static variables in a constructor because it can only be initialized once
        static int person_count;
        
        //Constructor: Called on the creation of the object, used for initialization
        Person() 
        {
            cout << "Constructor" << endl;
            person_count++; //every new object, count is increased

        }
        
        //Destructor: Called on destruction of object, used for clean up
        ~Person()
        {
            cout << "Destructor" << endl;
        }
        
        //It is a member function that is used to access only static data members. 
        //It cannot access non-static data members not even call non-static member functions. 
        //It can be called even if no objects of the class exist.
        static void PersonTotal()
        {
            cout << person_count << endl;
        }
        
};

int Person::person_count = 0; //set value outside of class

int main()
{
    cout << Person::person_count << endl; //has class scope so you dont need an object first
    
    Person eric;
    Person::PersonTotal(); //You can just use scope operator Person:: instead of eric.PersonTotal
    
    Person evan;
    Person::PersonTotal();
	
	//You can also initialize a static object but keep in mind that this will allocate storage space and then
	//it wil have scope til the end of the program.
	//THE DESTRUCTOR IS ONLY CALLED WHEN main() FUNCTION EXITS!!!
    static Person sam;
	Person::PersonTotal();
	
    return 0;
}
