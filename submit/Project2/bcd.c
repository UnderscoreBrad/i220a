#include "bcd.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


/** Return BCD encoding of binary (which has normal binary representation).
 *
 *  Examples: binary_to_bcd(0xc) => 0x12;
 *            binary_to_bcd(0xff) => 0x255
 *
 *  If error is not NULL, sets *error to OVERFLOW_ERR if binary is too
 *  big for the Bcd type, otherwise *error is unchanged.
 */
Bcd
binary_to_bcd(Binary value, BcdError *error)
{
	Binary maxVal=0;
	for(int i = 0; i < MAX_BCD_DIGITS; i++){
		maxVal+=9;
		maxVal*=10;
	}
	if(value > maxVal){
		*error = OVERFLOW_ERR;
		return 0;		
	}
  Binary n = value;
  unsigned length = 1;
    while (n > 9) {
        n /= 10;
        length++;
    }
    n = value;
    Binary rtn = 0;
    Binary arr[length];
    for(unsigned i = 0; i < length; i++){
    arr[i] = value % (Bcd)(pow(10,i+1));
    value -= arr[i];
    arr[i] /= pow(10,i);
    arr[i] <<= (4*i);
    rtn += arr[i];
  }
  if(n > rtn){
    *error = OVERFLOW_ERR;
		return 0;
  }
  return rtn;
}

/** Return binary encoding of BCD value bcd.
 *
 *  Examples: bcd_to_binary(0x12) => 0xc;
 *            bcd_to_binary(0x255) => 0xff
 *
 *  If error is not NULL, sets *error to BAD_VALUE_ERR if bcd contains
 *  a bad BCD digit.
 *  Cannot overflow since Binary can represent larger values than Bcd
 */
Binary
bcd_to_binary(Bcd bcd, BcdError *error)
{
  Bcd n = bcd;
  unsigned length = 1;
    while (n > 9) {
        n /= 10;
        length++;
    }
		n = bcd;
    Binary rtn = 0;
		BcdError *e = 0;
    for(unsigned i = 0; i < length; i++){
   	Binary temp = bcd >> (4*i);
    temp %= 16;
    if(temp > 9){
      *error = BAD_VALUE_ERR;
			return 0;
    }
    bcd -= temp;
    rtn += temp*(pow(10,i));
		if(n == binary_to_bcd(rtn, e)){
		//This takes more time but was the only easy solution
		//to a problem that made the program keep adding
		//past finding the right value of the BCD
		return rtn;
	}
  }
  return rtn;
}

/** Return BCD encoding of decimal number corresponding to string s.
 *  Behavior undefined on overflow or if s contains a non-digit
 *  character.  Sets *p to point to first non-digit char in s.
 *  Rougly equivalent to strtol().
 *
 *  If error is not NULL, sets *error to OVERFLOW_ERR if binary is too
 *  big for the Bcd type, otherwise *error is unchanged.
 */
Bcd
str_to_bcd(const char *s, const char **p, BcdError *error)
{
	char *ptr;
  Bcd rtn = strtol(s, &ptr, 10);
  rtn = binary_to_bcd(rtn, error);
	*p = ptr;
  return rtn;
}

/** Convert bcd to a NUL-terminated string in buf[] without any
 *  non-significant leading zeros.  Never write more than bufSize
 *  characters into buf.  The return value is the number of characters
 *  written (excluding the NUL character used to terminate strings).
 *
 *  If error is not NULL, sets *error to BAD_VALUE_ERR is bcd contains
 *  a BCD digit which is greater than 9, OVERFLOW_ERR if bufSize bytes
 *  is less than BCD_BUF_SIZE, otherwise *error is unchanged.
 */
int
bcd_to_str(Bcd bcd, char buf[], size_t bufSize, BcdError *error)
{
  if(bufSize < BCD_BUF_SIZE){
    *error = OVERFLOW_ERR;
		return 0;
  }
	if(bcd==0){
		buf[0] = '0';
		buf[1] = '\0';
		return 1;
	}
  Binary n = bcd;
  unsigned length = 1;
    while (n > 9) {
        n /= 10;
        length++;
    }
    n = bcd;
    Binary arr[length];
    for(unsigned i = 0; i < length; i++){
    arr[i] = bcd >> (BCD_BITS*i);
    arr[i] %= 16;
    if(arr[i] > 9 || arr[i] < 0){
      *error = BAD_VALUE_ERR;
			return 0;
    }
  }
  for(unsigned i = 1; i < length; i++){
    buf[i-1] = arr[length-i-1]+48;
  }
	buf[length] = '\0';
	if(length==1){
		return 1;
	}		
	//This gives a weird failure in the testers
  return (length-1);
}

/** Return the BCD representation of the sum of BCD int's x and y.
 *
 *  If error is not NULL, sets *error to to BAD_VALUE_ERR if x or y
 *  contains a BCD digit which is greater than 9, OVERFLOW_ERR on
 *  overflow, otherwise *error is unchanged.
 */
Bcd
bcd_add(Bcd x, Bcd y, BcdError *error)
{
  Binary xb = bcd_to_binary(x, error);
  Binary yb = bcd_to_binary(y, error);
  Binary add = xb + yb;
  Bcd rtn = binary_to_bcd(add, error);
  return rtn;
}

/** Return the BCD representation of the product of BCD int's x and y.
 *
 * If error is not NULL, sets *error to to BAD_VALUE_ERR is x or y
 * contains a BCD digit which is greater than 9, OVERFLOW_ERR on
 * overflow, otherwise *error is unchanged.
 */
Bcd
bcd_multiply(Bcd x, Bcd y, BcdError *error)
{
  Binary xb = bcd_to_binary(x, error);
  Binary yb = bcd_to_binary(y, error);
  Binary mult = xb * yb;
  Bcd rtn = binary_to_bcd(mult, error);
  return rtn;
}
