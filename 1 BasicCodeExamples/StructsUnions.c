/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
// Online Compiler: https://www.onlinegdb.com/online_c++_compiler

#include <iostream>

using namespace std;

//typedef: creates a new name for an existing thing
typedef int my_int; //can use my_int for int now

////////////////////////////////////////
// Structs and typedef
////////////////////////////////////////

struct address
{
    int housenum;
    string street;
    string city;
};

struct student
{
    int idnum;
    int age;
    address addr; //nested struct
}eric, mike; //Makes eric and mike structs right away, struct needs ; at end    

typedef student student_t; //we are making the new name student_t that can be used in place of student

//or you can declare like this
typedef struct student1
{
    int idnum;
    int age;
    address addr; //nested struct
}student_tt; //we can now use student_tt instead of student1


void print_struct(student_t structure)
{
    cout << "kyle idnum: " << structure.idnum << endl;
    cout << "kyle age: " << structure.age << endl;
}

void print_struct2(student_t *structure)
{
    cout << "eric idnum: " << structure->idnum << endl; //need to use -> for pointer
    cout << "eric age: " << structure->age << endl;
}


////////////////////////////////////////
// Unions
////////////////////////////////////////
//Similar to a struct but you can only store the value
//in only one member at a time because the memory is shared
union employee
{
    int id;
    float salary;
}tim, john; 



int main()
{
    //Structs
    cout << "Hello World" << endl;
    
    student evan, todd; //Makes new structs
    
    student_t kyle = {12, 24}; //Using typedef
    cout << "kyle idnum: " << kyle.idnum << endl;
    cout << "kyle age: " << kyle.age << endl;
    
    student_tt jim = {15, 32};
    cout << "jim idnum: " << jim.idnum << endl;
    cout << "jim age: " << jim.age << endl;
    
    eric.idnum = 10;
    eric.age = 31;
    cout << "eric idnum: " << eric.idnum << endl;
    cout << "eric age: " << eric.age << endl;
    
    evan = {11,25};
    cout << "evan idnum: " << evan.idnum << endl;
    cout << "evan age: " << evan.age << endl;
    
    print_struct(kyle);
    
    //Using pointers
    student *ericptr;
    ericptr = &eric;
    ericptr->idnum = 5;
    
    print_struct2(ericptr); //or print_struct2(&eric);
    
    //nested struct address
    eric.addr.housenum = 123;
    cout << "eric address housenum: " << eric.addr.housenum << endl;
    
    ericptr->addr.housenum = 456;
    cout << "eric address housenum: " << eric.addr.housenum << endl;
    
    
    
    //Unions
    employee sam, fred;
    fred.id = 25;
    cout << fred.id << endl;
    tim.salary = 10;
    cout << tim.salary << endl; //prints 10
    cout << fred.id << endl; //should also print 10 because memory is shared
    
    return 0;
}

//Typedef example
//You can use typedef to give a new name to an existing typedef
typedef int kilometer //we are naming int kilometer, now we can replace int with kilometer in our code
int main()
{
	kilometer newyork, chicago, miami; //we are using kilometer instead of int to make it more readable
	
	newyork = 300;
	chicago = 500;
	miami = 1900;
	
	return 0;
}

