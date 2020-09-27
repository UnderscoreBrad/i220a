#include "hamming.h"

#include <math.h>
#include <assert.h>

/**
  All bitIndex'es are numbered starting at the LSB which is given index 1
  ** denotes exponentiation; note that 2**n == (1 << n)
*/

/** Return bit at bitIndex from word. */
static inline unsigned
get_bit (HammingWord word, int bitIndex)
{
    assert(bitIndex>=0); //line had to be changed due to implementation
    unsigned rtn = (word & ( 1 << bitIndex-1 )) >> bitIndex-1;
    return rtn;
}

/** Return word with bit at bitIndex in word set to bitValue. */
static inline HammingWord
set_bit (HammingWord word, int bitIndex, unsigned bitValue)
{
  assert (bitIndex > 0);
  assert (bitValue == 0 || bitValue == 1);
  HammingWord newBit = (1 << bitIndex-1);
  if (bitValue == 1 && get_bit (word, bitIndex) == 0)
    {
      word += newBit;
    }
  else if(bitValue == 0 && get_bit (word, bitIndex) == 1)
    {
      word -= newBit;
    }
  return word;
}

/** Given a Hamming code with nParityBits, return 2**nParityBits - 1,
 *  i.e. the max # of bits in an encoded word (# data bits + # parity
 *  bits).
 */
static inline unsigned
get_n_encoded_bits (unsigned nParityBits)
{
  unsigned bitCount = 1ull << nParityBits - 1;
  return bitCount;
}

/** Return non-zero if bitIndex indexes a bit which will be used for a
 *  Hamming parity bit; i.e. the bit representation of bitIndex
 *  contains only a single 1.
 */
static inline int
is_parity_position (int bitIndex)
{
  assert (bitIndex > 0);
  int x = (bitIndex != 0) && ((bitIndex & (bitIndex - 1)) == 0);
  return x;
}

/** Return the parity over the data bits in word specified by the
 *  parity bit bitIndex.  The word contains a total of nBits bits.
 *  Equivalently, return parity over all data bits whose bit-index has
 *  a 1 in the same position as in bitIndex.
 */
static int
compute_parity (HammingWord word, int bitIndex, unsigned nBits)
{
  assert (bitIndex > 0);
  unsigned currentParityVal = 0;
  unsigned newParityVal = 0;
  unsigned wordLength = get_n_encoded_bits(nBits);
  for (unsigned i = 1u; i <= wordLength; i++)
    {
      if (!(is_parity_position (i)) && (i & bitIndex))
	{
	  unsigned currBit = get_bit (word, i);
	  currentParityVal = currentParityVal ^ currBit;
	}
    }
  return currentParityVal;
}

/** Encode data using nParityBits Hamming code parity bits.
 *  Assumes data is within range of values which can be encoded using
 *  nParityBits.
 */
HammingWord
hamming_encode (HammingWord data, unsigned nParityBits)
{
  HammingWord tempData = data;
  unsigned wordLength = get_n_encoded_bits (nParityBits);
  for (unsigned i = 1; i <= wordLength; i++)
    {
      if (is_parity_position (i))
	{
	  HammingWord addIn = tempData % (1u << i);
	  addIn <<= 1;
	  tempData >>= i;
	  tempData <<= i + 1;
	  tempData += addIn;
	  if (i > 3 && get_bit (tempData, i+1) == 1
	      && !get_bit (tempData, i) == 0
	      && is_parity_position (i-1) == 0)
	    {
	      tempData = set_bit (tempData, i - 1, 1);
	      tempData = set_bit (tempData, i, 0);
	    }
	}
    }
  for (unsigned i = 0; i < nParityBits; i++)
    {
      int j = (1 << i);
	  int x = compute_parity (tempData, j, wordLength);
	  if (x == 1)
	    {
	      tempData = set_bit (tempData, j, x);
	    }
	
    }
  return tempData;
}

/** Decode encoded using nParityBits Hamming code parity bits.
 *  Set *hasError if an error was corrected.
 *  Assumes that data is within range of values which can be decoded
 *  using nParityBits.
 */
HammingWord
hamming_decode (HammingWord encoded, unsigned nParityBits,
		int *hasError)
{
  unsigned wordLength = get_n_encoded_bits (nParityBits);
  HammingWord tempData = encoded;
  HammingWord parityErrors[nParityBits];
  for (unsigned i = 0; i < nParityBits; i++)
    {
      unsigned j = 1u << i;
      unsigned expected = compute_parity (tempData, j, wordLength);
      if (expected != get_bit (tempData, j))
	{
	  parityErrors[i] = j;
	}
      else
	{
	  parityErrors[i] = 0;
	}
    }
    unsigned errorPlace = 0;
    for(unsigned i = 0; i < nParityBits; i++){
        errorPlace = errorPlace | parityErrors[i];
    }
    unsigned x = 0;
    if(get_bit(tempData, errorPlace)==0){
        x = 1;
    }
    tempData = set_bit(tempData, errorPlace, x);
    
        //break here plz
    for (unsigned i = 1u; i <= wordLength; i++)
    {
      if (is_parity_position (i))
	{
	  tempData = set_bit(tempData,i,0);
    }
    }
    for(unsigned i = 1u; i <= wordLength; i++){
        if(is_parity_position){
            HammingWord addIn = tempData % (1<<(i+1));
            tempData >>= i;
            tempData <<= i-1;
            tempData += addIn;
        }
    }
  return tempData;
}