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
    assert(bitIndex>0);
    unsigned rtn = (word & ( 1 << bitIndex-1 )) >> bitIndex-1;
    return rtn;
}

/** Return word with bit at bitIndex in word set to bitValue. */
static inline HammingWord
set_bit(HammingWord word, int bitIndex, unsigned bitValue)
{
  assert(bitIndex > 0);
  assert(bitValue == 0 || bitValue == 1);
  HammingWord newBit = 1 << bitIndex-1;;
  HammingWord tempWord = word;
  if(bitValue == 1){
    tempWord += newBit;
  }else{
      tempWord -= newBit;
  }
  return tempWord;
}

/** Given a Hamming code with nParityBits, return 2**nParityBits - 1,
 *  i.e. the max # of bits in an encoded word (# data bits + # parity
 *  bits).
 */
static inline unsigned
get_n_encoded_bits(unsigned nParityBits)
{
  unsigned bitCount = 1ull << nParityBits -1;
  return bitCount;
}

/** Return non-zero if bitIndex indexes a bit which will be used for a
 *  Hamming parity bit; i.e. the bit representation of bitIndex
 *  contains only a single 1.
 */
static inline int
is_parity_position(int bitIndex)
{
  assert(bitIndex > 0);
  int x = (bitIndex != 0) && ((bitIndex & (bitIndex - 1)) == 0);
  return x;
}

/** Return the parity over the data bits in word specified by the
 *  parity bit bitIndex.  The word contains a total of nBits bits.
 *  Equivalently, return parity over all data bits whose bit-index has
 *  a 1 in the same position as in bitIndex.
 */
static int
compute_parity (unsigned long long word, int bitIndex, unsigned nBits)
{
  assert (bitIndex > 0);
  unsigned currentParityVal = 0;
  unsigned newParityVal = 0;
  unsigned wordLength = get_n_encoded_bits(nBits);
  for (unsigned i = 1u; i <= wordLength; i++)
    {
        //printf("%d&%d %d\n", i, bitIndex, (i & bitIndex));
      if (!(is_parity_position (i)) && (i & bitIndex))
	{
	  unsigned currBit = get_bit (word, i);
        printf("%d %d^%d=%d\n", i, currentParityVal,currBit,currentParityVal^currBit);
	  currentParityVal = currentParityVal ^ currBit;
	}
    }
  return currentParityVal;
}

/** Encode data using nParityBits Hamming code parity bits.
 *  Assumes data is within range of values which can be encoded using
 *  nParityBits.
 */
unsigned long long
hamming_encode (unsigned long long data, unsigned nParityBits)
{
  unsigned long long tempData = data;
  unsigned wordLength = get_n_encoded_bits (nParityBits);
  for (unsigned i = 1; i <= wordLength; i++)
    {
      if (is_parity_position (i))
	{
	  unsigned long long addIn = tempData % (1u << i);
	  addIn <<= 1;
	  tempData >>= i;
	  tempData <<= i + 1;
	  tempData += addIn;
	  if (i > 3 && get_bit (tempData, i+1) == 1
	      && !get_bit (tempData, i) == 0
	      && is_parity_position (i - 1) == 0)
	    {
	      tempData = set_bit (tempData, i - 1, 1);
	      tempData = set_bit (tempData, i, 0);
	    }
	}
    }
  printf (" == %llu == \n", tempData);
  int y = compute_parity(tempData, 1, wordLength);
 // printf("aborts before here?");
  if(y==1) tempData = set_bit(tempData,1,y);
  for (unsigned i = 1; i <= nParityBits; i++)
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
hamming_decode(HammingWord encoded, unsigned nParityBits,int *hasError)
{
  unsigned errorParBits[nParityBits];
  unsigned wordLength = get_n_encoded_bits(nParityBits);
  for(int i = 0; i<nParityBits; i++){
    unsigned j = 1u << i;
       if(compute_parity(encoded,j,wordLength)!=get_bit(encoded,j)){
         errorParBits[i] = j;
       }
  }
  for(int i = 0; i<wordLength; i++){
    if(is_parity_position(i)==0){
      unsigned parityOR = 0u;
      for(unsigned j = 0u; j<nParityBits; j++){
        if(j!=0u&&errorParBits[j]==j){
          parityOR = parityOR | (j&i);
        if(parityOR==i){
          set_bit(encoded,i,~(get_bit(encoded,i)));
        }
      }
    }
  }
  for(int i = 0; i < nParityBits; i++){
    if(is_parity_position(i)){
      HammingWord rem = encoded % (1u<<i);
      HammingWord
  }
  return 0;
}