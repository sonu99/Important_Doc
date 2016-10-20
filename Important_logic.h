1> ----> if number is divisible by 8 then last 3 digit should be 000.
   ----> test it for checking, divisible by 8 or not..if (((x >> 3) << 3) == x) divisibleBy8 = true;
2> transpose of matrix in 1D
  http://stackoverflow.com/questions/32404915/how-to-transpose-2d-matrix-stored-as-c-1d-array
#include <stdio.h>

int main() {
    int array[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    int i, j;

    for (j = 0; j < 3; ++j) {
        for (i = 0; i < 3; ++i) {
            printf("%d ", array[j + i * 3]);
        }
        printf("\n");
    }
}
3> http://www.dailyfreecode.com/code/sorting-link-list-2425.aspx


4> reverse the string..

Ans: 
reverse(char *str, int len){
   int i=0;
   while(i<len)
      {
         temp = str[i];
         str[i++] = str[len];
         str[len--] = str[i];
      }
   return str;
}

5> swap the number using bitwise 
Ans: 
void swap(int *x, int *y)
{
    *x = *x ^ *y;  //X^X ----> 0(xor of same no will be zero)......and x^0 = x....
    *y = *x ^ *y;
    *x = *x ^ *y;

}




6> diffrence between linked list and array.
   
