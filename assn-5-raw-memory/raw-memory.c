/**
 * Simple C test programs for Assignment 5: Raw Memory
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>

int main(int argc, char **argv)
{
  /**
   * Problem 1: Binary numbers and bit operations
   */
  printf("Problem 1:\n");

  // Test adding large signed an unsigned shorts
  printf("\tSize of a short: %lu bytes \n", sizeof(short));
  printf("\tMaximum value of unsigned short: %d\n", USHRT_MAX);

  unsigned short largeUnsShort1 = 60000;
  unsigned short largeUnsShort2 = 50000;
  unsigned short addLargeUnsShort = largeUnsShort1 + largeUnsShort2;
  printf("\tAdd large unsigned shorts: %hu + %hu = %hu\n", largeUnsShort1,
      largeUnsShort2, addLargeUnsShort);
  printf("\tResult: No error is reported on the overflow.  The resulting value\n\tis (%d + 1) smaller than the correct value.\n", 
      USHRT_MAX);
  
  printf("\n\tRange of signed short: %d to %d\n", SHRT_MIN, SHRT_MAX);
  short largeShort1 = 20000;
  short largeShort2 = 25000;
  short addLargeShort = largeShort1 + largeShort2;
  printf("\tAdd large signed shorts: %d + %d = %d\n", largeShort1, largeShort2,
      addLargeShort);
  printf("\tResult: An incorrect large negative value, representing the distance\n\tfrom the lowest value that the integer overflowed: \n\t(%d + (%d + %d - %d - 1)).\n",
      SHRT_MIN, largeShort1, largeShort2, SHRT_MAX);

  // Test assigning to/from different size integers
  unsigned int largeUnsInt = pow(2, 20) + pow(2, 2);
  unsigned short smallUnsShort = pow(2, 4);
  unsigned int smallToLarge = (unsigned int) smallUnsShort;
  unsigned short largeToSmall = (unsigned short) largeUnsInt;
  printf("\n\tLarge unsigned integer: %d = 2^20 + 2^2\n", largeUnsInt);
  printf("\tSmall unsigned short: %d = 2^4\n", smallUnsShort);
  printf("\tAssign from small to large: %d\n", smallToLarge);
  printf("\tThis direction works fine since the large integer can\n\taccomodate all the bits of the small integer.\n");
  printf("\tAssign from large to small: %d\n", largeToSmall);
  printf("\tThis direction does not work since the small integer does\n\tnot have enough bits.");
  printf("\tThe bit corresponding to the 2^20 value in the\n\tlarge integer is truncated.\n");

  // Bitwise operations
  printf("\n\tTo determine the remainder of a number when dividing by four\n\tusing bitwise operations, AND the number with 2^0 + 2^1 = 3\n");
  unsigned char mod4test = 39;
  unsigned char mod4mask = 3;
  unsigned char mod4result = 39 & mod4mask;
  printf("\tExample: %d %% 4 = %d\n\n", mod4test, mod4result);

  printf("\tTo determine if a number would lose data converting to a short\n");
  printf("\tor char, you can AND the value with the maximum short/char value,\n");
  printf("\tthen see if that value is equal to the original value.\n");
  printf("\tExample int to short: %d & %d = %d != %d\n", largeUnsInt, USHRT_MAX,
     largeUnsInt & USHRT_MAX, largeUnsInt); 

  printf("\n\tTo convert a value from positive to negative using bitwise operations,\n");
  printf("\ttake the NOT of a number and add 1.\n");
  char posToNeg = 54;
  printf("\tExample: ~%d + 1 = %d\n", posToNeg, ~posToNeg + 1); 

  printf("\n\tTo construct a simple encryption scheme, you could XOR a program with\n");
  printf("\ta key value.  XORing the program again with the key would decrypt it.\n");

  /**
   * Problem 2: ASCII and extended ASCII
   *
   * Viewing the ascii.txt file in two systems, Mac and Linux, shows that the 
   * upper half of the ASCII range is not consistent between the two. This
   * implies that if an email or webpage containing extended ASCII characters
   * is sent between two different systems, the result could be incorrect.
   */

  /**
   * Problem 3: It's just bits and bytes
   */
  char *hi = strdup("hi!");
  int three = 3;

  printf("\nProblem 3:\n");
  printf("\tInterpreting \"hi!\" as an integer: %d\n", * (int *) hi);
  printf("\tInterpreting the integer bit pattern for 3 as a float: %.8f\n", 
      * (float *) &three);
 
  typedef struct { 
   int i;
  } foo;
  foo testFoo;

  printf("\tInterpret a 4-byte struct as though it were a float: %f\n",
     * (float *) &testFoo); 
   
  char *bigString = strdup("Four score and seven years ago our fathers brought forth on this continent...");
  int i;
  memcpy(&i, bigString, sizeof(int));
  printf("\tAssign an integer to be the first four letters of a string: %d\n",
      i);

  short s = 1;
  printf("\tIf big endian, this should be 0: %d\n", *  (unsigned char *) &s);
} 
