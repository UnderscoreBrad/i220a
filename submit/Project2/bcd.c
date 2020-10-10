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
  Binary n = value;
  int length = 1;
    if (n < 0) n = (n == INT_MIN) ? INT_MAX: -n;
    while (n > 9) {
        n /= 10;
        length++;
    }
    n = value;
    Binary rtn = 0;
    Binary arr[length];
    for(unsigned i = 0; i < length; i++){
    arr[i] = value % (unsigned)(pow(10,i+1));
    value -= arr[i];
    arr[i] /= pow(10,i);
    arr[i] <<= (4*i);
    rtn += arr[i];
  }
  if(n > rtn){
    printf("ERROR");
    *error = OVERFLOW_ERR;
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
  Binary n = bcd;
  int length = 1;
    if (n < 0) n = (n == INT_MIN) ? INT_MAX: -n;
    while (n > 9) {
        n /= 10;
        length++;
    }
    n = bcd;
    Binary rtn = 0;
    Binary arr[length];
    for(unsigned i = 0; i < length; i++){
    arr[i] = bcd >> (4*i);
    arr[i] %= 16;
    if(*error == NULL && arr[i] > 9){
      *error = BAD_VALUE_ERR;
    }
    bcd -= arr[i];
    rtn += arr[i]*(pow(10,i));
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
  //@TODO
  return 0;
}

/** Return the BCD representation of the sum of BCD int's x and y.
 *
 *  If error is not NULL, sets *error to to BAD_VALUE_ERR is x or y
 *  contains a BCD digit which is greater than 9, OVERFLOW_ERR on
 *  overflow, otherwise *error is unchanged.
 */
Bcd
bcd_add(Bcd x, Bcd y, BcdError *error)
{
  Binary xb = bcd_to_binary(x, error);
  Binary yb = bcd_to_binary(y, error);
  Binary add = x + y;
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
  Binary mult = x * y;
  Bcd rtn = binary_to_bcd(mult, error);
  return rtn;
}