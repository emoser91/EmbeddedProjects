/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
// Online Compiler: https://www.onlinegdb.com/online_c++_compiler

#include <iostream>

using namespace std;

////////////////////////////////////////
// Enums
////////////////////////////////////////
/* Enumerated Types allow us to create our own symbolic names for 
   a list of related ideas. The key word for an enumerated type is enum. 
   We could also create an enumerated type to represent various security 
   levels so that our program could run a door card reader*/
   
//Will default as black_ops = 0, top_secret = 1, secret = 2, non_secret = 3
//You could also set members to specific values if you want to
enum Security_Levels 
{ 
  black_ops, 
  top_secret, 
  secret, 
  non_secret 
};       // don't forget the semi-colon ; 
 

int main()
{
    Security_Levels my_security_level = top_secret;
 
    if ( my_security_level == black_ops ) 
    { 
        printf("Welcome Mister Bond\n"); 
        // open_safe();  
        // open_door(); 
    } 
    else if ( my_security_level == top_secret ) 
    { 
        printf("Welcome Q\n"); 
        // open_door();  
    } 
    else if ( my_security_level == secret ) 
    { 
        printf("Please leave Now\n"); 
    } 
    else if ( my_security_level == non_secret ) 
    { 
        printf("Warning, The Police have been Called\n"); 
        printf("Surrender yourself to them immediately!\n"); 
        // call_police(); 
    } 
        
    return 0;
}
