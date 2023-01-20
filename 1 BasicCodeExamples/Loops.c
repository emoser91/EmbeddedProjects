/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
// Online Compiler: https://www.onlinegdb.com/online_c++_compiler

#include <iostream>

using namespace std;

////////////////////////////////////////
// Loops and Switch
////////////////////////////////////////

int main()
{
    //If Statements
    int a = 0;
    // a = 1; //other values to try out on the statement below
    // a = 2;
    if(a == 0)
    {
        cout << "If" << endl;
    }
    else if (a == 1)
    {
        cout << " Else If" << endl;
    }
    else
    {
        cout << "Else" << endl;
    }
    
    int b = 0;
    if((a == 0) && (b == 0))
    {
        cout << "a and b are both 0" << endl;
    }
    
    //Single Line if/else statement: Ternary Operator
    //variable = (condition) ? expressionTrue : expressionFalse;
    string var = (a == 0) ? "True" : "False";
    cout << var << endl;
    
    
    //For Loops
    for(int i = 0; i < 5; i++) //prints 0 1 2 3 and not 4 because of break at 3
    {
        cout << i << endl;
        
        if(i == 3)
            break; //stops looping
    }
    
    for(int i = 0; i < 5; i++) //prints 0 1 2 4 and not 3 because of continue at 3
    {
        if(i == 3)
            continue; //skip the rest of the execution and go on looping
            
        cout << i << endl;
    }
    
    
    //While Loops
    int n = 0;
    while(n < 5)
    {
        cout << n << endl; //prints 0 1 2 3 4
        n++;
    }
    
    
    //Do While Loops
    int j = 0;
    do
    {
        cout << j << endl; //prints 0 1 2 3 4
        j++;
    }while(j < 5);
    
    
    //Switch statement that steps through an array of values
    char grades[] = {'A', 'B', 'C', 'D', '0'}; //prints Nice Nice Okay Okay Invalid
    int length = sizeof(grades)/sizeof(grades[0]);
    
    for(int i = 0; i < length; i++)
    {
        switch (grades[i])
        {
            case 'A':
                cout << "Nice" << endl;
                break; //kicks program flow out of the switch
                
            case 'B':
                cout << "Nice" << endl;
                break;
            
            case 'C':
				//technically you can also put statements after 'C' case
            case 'D':
                cout << "Okay" << endl; //case 'C' and 'D' will both end here
				//when a case is matched, program will keep going until a break is found
				//does make it harder to read code though, it is called fall through logic
                break;
                
            default:
                cout << "Invalid" << endl; //case if no other cases are found
         }
    }
    
	//Ternary Operator: quick inline if statement
	int x = 5;
	int y = 2;
	int z = 0;
	
	// if(a == b) c = 10; //normal way but really condensed
	// else c = 5;
	
	z = (x == y) ? 10 : 5; // 10 if true and 5 if false 
	cout << "z: " << z << endl;
	
    //Infinite loop
    // while(1)
    // {
        
    // }
    // //or 
    // for(;;)
    // {
        
    // }
    
    
    
    return 0;
}
