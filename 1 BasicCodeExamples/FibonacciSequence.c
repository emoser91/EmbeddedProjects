/*
	The Fibonacci sequence is a sequence where the next term is the sum 
	of the previous two terms. The first two terms of the Fibonacci 
	sequence are 0 followed by 1.
	
	Fibonacci sequence: 0, 1, 1, 2, 3, 5, 8, 13, 21, ...
	
	
*/

#include <stdio.h>
int main() {

  int i, n;

  // initialize first and second terms
  int t1 = 0, t2 = 1;

  // initialize the next term (3rd term)
  int nextTerm = t1 + t2;

  // get no. of terms from user
  printf("Enter the number of terms: ");
  scanf("%d", &n);

  // print the first two terms t1 and t2
  printf("Fibonacci Series: %d, %d, ", t1, t2);

  // print 3rd to nth terms
  for (i = 3; i <= n; ++i) {
    printf("%d, ", nextTerm);
    t1 = t2;
    t2 = nextTerm;
    nextTerm = t1 + t2;
  }

  return 0;
}

/* 
	OpenAI GPT generated this next answer based on the prompt I gave itoa
	
	Prompt: write a program in c that will print out the first 10 values of the fibonacci series
	
	The result uses an array which turns out to be a really good answer. 
	
	Text explaination that the AI gave:
	This program declares an array fib of size 10 and initializes the first two values of the Fibonacci series (0 and 1) 
	to the first two elements of the array. It then uses a for loop to compute the remaining values of the series by 
	summing the previous two values in the array and storing the result in the current array element. Finally, it uses 
	another for loop to print out the elements of the fib array.
	Note that this program only computes the first 10 values of the Fibonacci series. To compute more values, you can 
	increase the size of the fib array and/or adjust the loop conditions in the for loops.
*/
#include <stdio.h>

int main(void) {
  int i;
  int fib[10];

  fib[0] = 0;
  fib[1] = 1;

  for (i = 2; i < 10; i++) {
    fib[i] = fib[i-1] + fib[i-2];
  }

  for (i = 0; i < 10; i++) {
    printf("%d ", fib[i]);
  }

  return 0;
}