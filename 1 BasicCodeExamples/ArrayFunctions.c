#include <iostream>

using namespace std;

int minNuminArray(int *array, int arrSize);
//or int minNuminArray(int array[], int arrSize); It is the same thing

int maxNuminArray(int *array, int arrSize);

int main()
{
    cout << "Hello World" << endl;
    
    int array[] = {10, 22, 34, 56, 33, 76, 44, 2, 35};
    
    //You can only use sizeof like this outside of function
    int arrSize = sizeof(array)/sizeof(array[0]);
    minNuminArray(array, arrSize);
    
    maxNuminArray(array, arrSize);
    
    return 0;
}

int minNuminArray(int *array, int arrSize)
{
    cout << "Finding Min Number in Array..." << endl;
    
    int min = array[0];
    
    for(int i = 0; i < arrSize; i++)
    {
        cout << "checking: " << array[i] << endl;
        
        if(array[i] < min)
        {
            min = array[i];
            cout << "New min found: " << min << endl;
        }
    }
    
    return min;
}

int maxNuminArray(int *array, int arrSize)
{
    cout << "Finding Max Number in Array..." << endl;
    
    int max = array[0];
    
    for(int i = 0; i < arrSize; i++)
    {
        cout << "checking: " << array[i] << endl;
        
        if(array[i] > max)
        {
            max = array[i];
            cout << "New max found: " << max << endl;
        }
    }
    
    return max;
}

int countNumofOccurrencesinArray(int *array, int arrSize)
{
    cout << "Finding Number of Occurrences in Array..." << endl;
    
    int max = array[0];
    
    for(int i = 0; i < arrSize; i++)
    {
        cout << "checking: " << array[i] << endl;
        
        if(array[i] > max)
        {
            max = array[i];
            cout << "New max found: " << max << endl;
        }
    }
    
    return max;
}