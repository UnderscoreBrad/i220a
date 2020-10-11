#include "bcd.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


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
	//compute maximum value this BCD can represent
	Binary maxVal=9;
	for(int i = 1; i < MAX_BCD_DIGITS; i++){
		maxVal*=10;
		maxVal+=9;
	}

	//Preliminary overflow check
	if(value > maxVal){
		*error = OVERFLOW_ERR;
		return 0;		
	}

	//Find length of value
  Binary n = value;
  unsigned length = 1;
    while (n > 9) {
        n /= 10;
        length++;
    }
		
    n = value; //reset n to value for second overlow check
		
		//Convert value to BCD, per decimal column
    Binary rtn = 0;
    for(unsigned i = 0; i < length; i++){
    Binary temp = value % (Bcd)(pow(10,i+1)); //store decimal in temp
    value -= temp; //remove value below temp (not strictly necessary)
    temp /= pow(10,i); //remove value above selected digit
    temp <<= (BCD_BITS*i); //shift to correct BCD place
    rtn += temp;	
    }
	
	//secondary overflow check, catch-all
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
	//determine length of bcd for the loop
  Bcd n = bcd;
  unsigned length = 1;
    while (n > 9) {
        n /= 16;
        length++;
    }
		
    Binary rtn = 0;
    for(unsigned i = 0; i < length; i++){
   	Binary temp = bcd >> (BCD_BITS*i);	//Shift to correct 4-bit segment
    temp %= 16; //remove larger 4 bit segments
    if(temp > 9){ //error check to make sure the digit isn't > 9
      *error = BAD_VALUE_ERR;
			return 0;
    }
    bcd -= temp;	//not strictly necessary
    rtn += temp*(Binary)(pow(10,i));	//add digit in correct place of rtn
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
	//this was the simplest solution
	//and it's portable!
	char *p2;
  Bcd rtn = strtoll(s, &p2, 10);
  rtn = binary_to_bcd(rtn, error);
	*p = p2;
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
	//check for buffer overflow
  if(bufSize < BCD_BUF_SIZE){
    *error = OVERFLOW_ERR;
		return 0;
  }
	//quick, easy zero case
	//also fixes error where bcd 0 returns ""
	if(bcd==0){
		buf[0] = '0';
		buf[1] = '\0';
		return 1;
	}
	//find length of bcd
  Binary n = bcd;
  unsigned length = 1;
    while (n > 9) {
        n /= 10;
        length++;
    }

		//Adds each BCD (as decimal) Digit straight to an array		
    for(unsigned i = 0u; i <+ length; i++){
    Binary temp = bcd >> (BCD_BITS*i);
    temp %= 16u;
    if(temp > 9u || temp < 0u){
      *error = BAD_VALUE_ERR;
			return 0u;
    }
  }
  sprintf(buf, "%llx", (unsigned long long) bcd);
  return (strlen(buf));
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
	//Whether you like it or not, this was the simplest implementation.
	//converts x and y to binary
  Binary xb = bcd_to_binary(x, error);
  Binary yb = bcd_to_binary(y, error);
	//adds as binary
  Binary add = xb + yb;
	//reconverts to BCD
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
	//Whether you like it or not, this was the simplest implementation.
	//converts x and y to binary
  Binary xb = bcd_to_binary(x, error);
  Binary yb = bcd_to_binary(y, error);
	//multiplies as binary
  Binary mult = xb * yb;
	//reconverts to BCD
  Bcd rtn = binary_to_bcd(mult, error);
  return rtn;
}
