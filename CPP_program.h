--------------------------------------------------------------------------
#include <iostream.h>

int mult (int x, int y) {
  int result;
  result = 0;
  while (y != 0) {
    result = result + x;
   y = y - 1;
  }
  return(result);
}

int main () {
  int x, y;
  cout << "Enter two natural numbers: ";
  cin >> x >> y;
  cout << x << " * " << y << " = " << mult(x,y) << endl;
  return(0);
}
------------------------------------------------------------------------
#include <iostream.h>

// Idea of the Russian Peasant Method (as old as 1700 B.C.)
// x * n = 2x * (n/2) if n even
//       = x + x * (n-1) if n odd

int fastmult (int x, int y) {
  int result;
  result = 0;
  while (y != 0) {
    if (y % 2 == 0) {
      x = 2*x;
      y = y/2;
    }
    else {
      result = result + x;
      y = y-1;
    }
  }
  return(result);
}

int main () {
  int x, y;
  cout << "Enter two natural numbers: ";
  cin >> x >> y;
  cout << x << " * " << y << " = " << fastmult(x,y) << endl;
  return(0);
}
---------------------------------------------------------------------------------------
//program for genrating calander...

#include <iostream>
#include <iomanip>
names 
bool isLeapYear( unsigned int& ); //checks if 'year' is leap year
unsigned int firstDayOfJanuary( unsigned int& year );
unsigned int numOfDaysInMonth( unsigned int , unsigned int& ); // takes the number of the month, and 'year' as arguments
void printHeader( unsigned int ); //takes the number of the month, and the first day, prints, and updates
void printMonth( unsigned int , unsigned int& ); //takes number of days in month, and reference to 'firstDayInCurrentMonth' so it updates after every call
void skip( unsigned int ); //prints the specified amount of spaces
 
int main() {
    unsigned int year , firstDayInCurrentMonth;
    std::cout << "Calendar year? ";
    std::cin >> year;
    std::cout << "\n";
    firstDayInCurrentMonth = firstDayOfJanuary( year );
    skip(9);
    std::cout << year << "\n";
    for ( unsigned int currentMonth = 1 ; currentMonth <= 12 ; currentMonth++ ) {
        printHeader( currentMonth );
        printMonth( numOfDaysInMonth( currentMonth , year ) , firstDayInCurrentMonth );
        std::cout << "\n\n\n";
    }
    std::cout << "Press Enter to Exit...";
    std::cin.ignore();
    std::cin.get();
}
 
bool isLeapYear( unsigned int& year ) { //if number is multiple of 4 and is either multiple of 400 or not multiple of 100, is leap year
    return ( year % 4 == 0 ) && ( year % 100 != 0 || year % 400 == 0 );
}
 
unsigned int firstDayOfJanuary( unsigned int& year ) {
    /* "( 97 * year - 97 ) / 400" is the simplification of:
 
    x1 = (year - 1)/ 4;
    x2 = (year - 1)/ 100;
    x3 = (year - 1)/ 400;
    day_start = (year + x1 - x2 + x3) % 7;
 
    after each value is plugged in
     */
 
    return ( year + ( 97 * year - 97 ) / 400 ) % 7;
}
 
unsigned int numOfDaysInMonth( unsigned int m , unsigned int& year ) {
    if ( m == 2 )
        return isLeapYear( year ) ? 29 : 28; //if month is February, return correct number based on whether it is leap year
    else
        return 30 + ( m % 2 ); //otherwise return 31 if month number is odd, 30 if month number is even
}
 
void printHeader( unsigned int m ) {
    skip( 7 );
 
    if ( m == 1 ) std::cout << "January";
    else if ( m == 2 ) std::cout << "February";
    else if ( m == 3 ) std::cout << "March";
    else if ( m == 4 ) std::cout << "April";
    else if ( m == 5 ) std::cout << "May";
    else if ( m == 6 ) std::cout << "June";
    else if ( m == 7 ) std::cout << "July";
    else if ( m == 8 ) std::cout << "August";
    else if ( m == 9 ) std::cout << "September";
    else if ( m == 10 ) std::cout << "October";
    else if ( m == 11 ) std::cout << "November";
    else if ( m == 12 ) std::cout << "December";
 
    std::cout << "\n S  M  T  W  T  F  S" << "\n";
    std::cout << "____________________" << "\n";
}
 
void skip( unsigned int i ) {
    while ( i > 0 ) {
        std::cout << " ";
        i--;
    }
}
 
void printMonth( unsigned int numDays, unsigned int &weekDay ) {
    skip( 3 * weekDay ); //3 is width of a calendar number
    for ( unsigned int day = 1 ; day <= numDays ; day++ ) {
        std::cout << std::setw(2) << day << " ";
 
        if ( weekDay == 6 ) {
            std::cout << "\n";
            weekDay = 0;
        }
        else
            weekDay++;
    }
}
