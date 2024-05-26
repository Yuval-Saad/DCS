kinds of function invoking:
Clause (1) shows template of function with a input argument, without any return value.
Clause (2) shows template of function with a return value, without any input argument.
Note: we can combine these two kinds in order to get function with at least one argument and one return value.


1) HLL signature: 

   void Print2LEDs(int ledsVal);

   MSP430 Assembly equivalent:
 
   push   ledsVal       // argument loading to stack
   call   #Print2LEDs   // function invoking

2) HLL signature: 

   int ReadSWs();

   MSP430 Assembly equivalent:
 
   call   #ReadSWs      // function invoking
   pop    Ri            // retrieve the return value from stack 
