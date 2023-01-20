/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
// Online Compiler: https://www.onlinegdb.com/online_c++_compiler

#include <iostream>

using namespace std;

////////////////////////////////////////
// Classes C++
////////////////////////////////////////

//A class is a blueprint or template from which objects are created. 
//An Object is an instance of a class.
class Person
{
    //Private: only the methods of the class have access
    private:
        int age;
    
    //Public: can be accessed by everyone
    public:
        //Constructor: Called on the creation of the object, used for initialization
        Person() 
        {
            cout << "Constructor" << endl;
            age = 0;
            name = "noname";
        }
        
        //Constructor with default values
        // Person(string nameValue = "noname", int ageValue = 0)
        // {
        //     cout << "Constructor with Default Values" << endl;
        //     age = ageValue;
        //     name = nameValue;
        // }
        
        //Constuctor with built in memory allocation
        // Person(string nameValue, int ageValue)
        // {
        //     name = new string;
        //     age = new int;
        //     *name = namevalue;
        //     *age = ageValue;
        // }
        // //Destructor with built in memory deallocation
        // ~Person()
        // {
        //     delete name;
        //     delete age;
        // }
        
        //Overloading Constructor: Depending on the call during creation a different constructor might be Called
        Person(int ageValue)
        {
            cout << "Overloaded Constructor" << endl;
            age = ageValue;
            name = "nonname";
        }
        Person(string nameValue)
        {
            cout << "Overloaded Constructor2" << endl;
            age = 0;
            name = nameValue;
        }
        
        //Destructor: Called on destruction of object, used for clean up
        ~Person()
        {
            cout << "Destructor" << endl;
        }
        
        //Public varriables and methods/functions
        string name;
        
        void introduce()
        {
            cout << "Hello my name is: " << name << endl;
        }
        
        void greeting();
        
        void setAge(int value)
        {
            age = value;
        }
        
        void displayAge()
        {
            cout << "Age: " << age << endl;
        }
};

void Person::greeting() //Note: scope not needed if defined in .hpp file with class
{
    cout << "Greetings, my name is: " << name << endl;
}

int main()
{
    //Creating a new object
    Person eric;
    eric.name = "eric";
    eric.introduce();
    
    //Creating a new object with dynamically allocated memory
    Person *evan = new Person();
    evan->name = "evan";
    evan->introduce();
    
    //Calling function declared outside the object
    eric.greeting();
    
    //Calling functions to set private value
    eric.setAge(31);
    eric.displayAge();
    
    //Overloaded constructor examples
    Person todd(31); //uses the first overloaded constructor call
    Person mike("mike"); //uses the second overloaded constructor call
    
    //Default values constructor call example
    // Person tim();
    // Person john("john", 25);
    
    //Destructor Call example
    //Destructor automatically called if you initialize normally
    //If you dynamicaly allocate memory like for evan then it will not automatically get called
    //This is why you can see destructor called for eric,todd,mike,sam but not evan
    Person *sam = new Person();
    delete sam;
    
    
    //Using the Constructor and Destructors that with built in memory allocation
    // Person *tom = new Person("tom", 35); //one line creation
    // delete tom; //will call destructor
    
    
    
    return 0;
}
