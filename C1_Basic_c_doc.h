//Question--*****************************************************************************************************************************

//Q..What is C language?
	The C programming language is a standardized programming language developed in the early 1970s by Ken Thompson and Dennis Ritchie for 		use on the UNIX operating system. It has since spread to many other operating systems, and is one of the most widely used programming 		languages. C is prized for its efficiency, and is the most popular programming language for writing system software, though it is also 		used for writing applications. ... 

//Q..Compilation..How to reduce a final size of executable? 
     Size of the final executable can be reduced using dynamic linking for libraries. 
//****************************************************************************************************************************************

//Notes-----------------------------------------------------------------------------------------------------------------------------------
C is block structure language..
	->Each statement should be part of function Block.
		ex-
			{
				int a;
				printf("Hi...sonu singh\n");
			}
	Note:-If statement is out side of block then it won't execute during the execution.That is only for compiler consideration
		ex-
			int a;
			main()
				{}
	->Advantage of block structure language.
		1>simplicity:-   It will look very simple..and easy to understand the execution flow.
		2>reusebility:-  If we are using same piece of code in other place of program then better reuse that block using funtion call.
				ex-
					void main()
					{
						printf("1----%d\n",sum(23,24));
						printf("2----%d\n",sum(34,56));
						
					}
					int sum(int a,int b)
					{
						return(a+b);
					}

		3>Easy to debug:-We can find error very easilly,because it will redirect to function block.



2>Variable:- It is an identifier used for holding the data.
	
	syntex:-data_type var_name;
	ex:	int roll;
	note:-Size and range of variable depend on data type.
	     -Scope, life and memory section depend on storage class.

3>constant:- it is fixed value for compiler depending on data type memory will allocate and it will allocate in code section.
	
		ex-string-"hello";
		   char 'A';
		 
4>Data type--------Size of data type is processor dependent
	-integer------int-----4byte------%d
	-	     sort int---2byte----%hd
	             unsigned int--4byte---%u
	-character char---1byte--%c
	-Real------float--4byte---%f
		   double-8byte---%lf
		   long double-10/12byte...%llf
5>User define data type:
	-Array:-It is UDT..which is collection of same type of data. 
	-struct:-It is UDT..which is capable to store related field of same or diffrent data type.
	-union:-same as structure but diffrent is, it will hold only one data at same time. 
	-enum:-enumerated data type.

6> Floating repersentation:-floating point co-processor (IEEE754)
	float---sign(1) + exp(8 bit)  + significant(23 bit)
	double--  1     +    11       +     52
	1.*e+-y
        for more detail---->vector notes
//Notes--------------------------------------------------input/output-------------------------------------------------------
1>  getchar()--------------putchar()-------------------\n will not flash it will still available in to input buffer register.
2>  gets()-----------------puts()----------------------Convert '\n' to '\0' 
3>  scanf()----------------printf()---------------------same as getchar and putchar.
note:-getchar and scanf both have same type of data entry process.
Note:-printf will return number of character(Bytes) written on output console..ret=printf("%d",123);....ret=3.
Note:-scanf will return number of 
//Question-*********************************************************************************************************************************

//Q>What is the output of printf("%d")? 
	1. When we write printf("%d",x); this means compiler will print the value of x. But as here, there is nothing after %d
	so compiler will show in output window garbage value. 

	2. When we use %d the compiler internally uses it to access the argument in the stack (argument stack).
	Ideally compiler determines the offset of the data variable depending on the format specification string.
	Now when we write printf("%d",a) then compiler first accesses the top most element in the argument stack of the printf
	which is %d and depending on the format string it calculated to offset to the actual data variable in the memory which is to be printed.
	Now when only %d will be present in the printf then compiler will calculate the correct offset(which will be the offset to access the integer variable)
	but as the actual data object is to be printed is not present at that memory location
	so it will print what ever will be the contents of that memory location. 

	3. Some compilers check the format string and will generate an error without the proper number and type of arguments for things like 		printf(...) and scanf(...). 

//Q>printf() Function- What is the difference between "printf(...)" and "sprintf(...)"? 
Ans:-sprintf(...) writes data to the character array whereas printf(...) writes data to the standard output device. 
   
   #include<math.h>
   int main()
   {
       char str[80];

       sprintf(str, "Value of Pi = %f", M_PI);
       puts(str);
   
       return(0);
}

//**********************************************************************************************************************************************
//Notes----------------------------operator-----------------------------------------
->>>Operator:-operator are symbol which is meaningfull to compiler and it's operate on operant to give usefull result.
Base on no of operand
	1>unary (&,*,++, --)
	2>Binary (=,+,-)
	3>ternory (? :)

//Notes------------------------------------------------------------------------------------------------------------------
Relational--  <, >, <=, >=, ==, !=   -----Single condition check---return type True/False..

Logical----&&->AND6, ||->OR, !->NOT  -----for multiple condition---Return type True/Flase.

Bitwise----&, |, ^, <<, >>, ~  -----------Return Type is value but data is not modify.

Note:---Bitwise operator can't use with real data type.
//Question...****************************************************************************************************************************
//Q> Which bit wise operator is suitable for turning off a particular bit in a number? 
	The bitwise AND operator,
	ex:-(data &=~(1<<bit))
//Q> Which bit wise operator is suitable for putting on a particular bit in a number? 
	The bitwise OR operator. 
	ex:-data|=(1<<bit)
//***********************************************************************************************************************************
//Notes---------------------------------
>>2's(x)=-(x)
>>1's(x)=2's(x)-1

>>if number is -ve--- -3<<4=-3*2^4
>>-120>>3=-15
>>-5>>5=-1
//----------------------------------
Compound Assigment
a+=b;
Increment/decrement;
a= ++a + ++a + ++a---undefine because value will vary with compiler.

//Notes--------------------------------------------------------------------------------------------------------------------------
Control statement:
--------un conditional
return
break
continue
goto

Note:- you can use break only inside loop and switch case....it will send control to out of loop..  
Note:- you can use continue only inside loop...it will bypass the other statment which is written after the continue...and execute the loop
 
--------control statement
if
if---else
switch----case
Notes:-
//--------Iterative/loop
while
for
do----while
//Bit Operation-------------
//--------------------------
Test Bit-- 	(data>>bit)&1
Set Bit--- 	data|=(1<<bit)
Clear Bit- 	(data &=~(1<<bit))
compliment bit--data^=(1<<bit)
//----------------------------------------------------
tpye conversion----->char->int->(sign->unsign)->float->double


//Notes-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------Storage Class:---------------------------------------------------------------------------------
>>it is keyword which tells compiler about variable.like memory, life and scope
1>auto:-
	Life:- come to life when function call
	memory:- stack
	Scope:- Within the function,where it is define.
	initial value:-garbage
2>register:-
	Life:- come to life when function call
	memory:- register(it is compiler choise.either it will go to stack or register)
	Scope:- Within the function,where it is define.
	initial value:-garbage
3>extern:-
	Life:- start with process and end with process
	memory:- data section 
	Scope:- within the program and out side the program(module) 
	initial value:-zero
4>static:-
	Life:- start with process and end with process
	memory:- data section 
	Scope:- within the program if it is declared as global and out side the program(module) 
	initial value:-zero
Note:-we can use static and extern for function also...
	if you want to  protect your function form outside module or program then use static keyword....
	or else you want to share function to outside then use extern..
Note:-All global declaration are external declaration.

Note:- Uninitialized data will go in to BSS 

//Question--***********************************************************************************************************************************

//Q--What does static variable mean? 
	there are 3 main uses for the static.
	1. If you declare within a function:
	It retains the value between function calls

	2.If it is declared for a function name:
	By default function is extern..so it will be visible from other files if the function declaration is as static..it is invisible for the 	outer files 

	3. Static for global variables:
	By default we can use the global variables from outside files If it is static global..that variable is limited to with in the file 


//Q--What are the different storage classes in C?
Ans..C has three types of storage: automatic, static and allocated. 

	Variable having block scope and without static specifier have automatic storage duration. 

	Variables with block scope, and with static specifier have static scope. Global variables (i.e, file scope) with or without the the 
	static specifier also have static scope. 

	Memory obtained from calls to malloc(), alloc() or realloc() belongs to allocated storage class.

//Q..When should the register modifier be used? Does it really help? 
Ans..The register modifier hints to the compiler that the variable will be heavily used and should be kept in the CPU’s registers, if possible, 
	so that it can be accessed faster. 
	There are several restrictions on the use of the register modifier. 
	First, the variable must be of a type that can be held in the CPU’s register. This usually means a single value of a size less than or 	
	equal to the size of an integer. Some machines have registers that can hold floating-point numbers as well. 
	Second, because the variable might not be stored in memory, its address cannot be taken with the unary & operator. An attempt to do so 	
	is flagged as an error by the compiler. Some additional rules affect how useful the register modifier is. Because the number of
	registers is limited, and because some registers can hold only certain types of data (such as pointers or floating-point numbers),
	the number and types of register modifiers that will actually have any effect are dependent on what machine the program will run on. 
	Any additional register modifiers are silently ignored by the compiler. 
	Also, in some cases, it might actually be slower to keep a variable in a register because that register then becomes unavailable for 		other purposes or because the variable isn’t used enough to justify the overhead of loading and storing it. 
	So when should the register modifier be used? The answer is never, with most modern compilers. Early C compilers did not keep any 		variables in registers unless directed to do so, and the register modifier was a valuable addition to the language. C compiler design 		has advanced to the point, however, where the compiler will usually make better decisions than the programmer about which variables 		should be stored in registers. 
	In fact, many compilers actually ignore the register modifier, which is perfectly legal, because it is only a hint and not a directive.

//********************************************************************************************************************************************
Notes:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::Function::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
