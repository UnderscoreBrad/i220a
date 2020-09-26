#include "hamming.h"

#include <math.h>
#include <assert.h>

/**
  All bitIndex'es are numbered starting at the LSB which is given index 1
  ** denotes exponentiation; note that 2**n == (1 << n)
*/

/** Return bit at bitIndex from word. */
static inline unsigned
get_bit(HammingWord word, int bitIndex)
{

  assert(bitIndex > 0);
  HammingWord tempWord = word;
	tempWord = tempWord >> bitIndex;
	unsigned rtn = tempWord % 2;
  return tempWord;
}

/** Return word with bit at bitIndex in word set to bitValue. */
static inline HammingWord
set_bit(HammingWord word, int bitIndex, unsigned bitValue)
{
  assert(bitIndex > 0);
  assert(bitValue == 0 || bitValue == 1);
  HammingWord newBit = 0 + bitValue;
  HammingWord tempWord = word;
  newBit = newBit << bitIndex;
  tempWord = tempWord & newBit;
  return tempWord;
}

/** Given a Hamming code with nParityBits, return 2**nParityBits - 1,
 *  i.e. the max # of bits in an encoded word (# data bits + # parity
 *  bits).
 */
static inline unsigned
get_n_encoded_bits(unsigned nParityBits)
{
  unsigned bitCount = pow(2, nParityBits)-1;
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
  int x = (bitIndex!=0)&&(bitIndex&(bitIndex-1))==0;
  return x;
}

/** Return the parity over the data bits in word specified by the
 *  parity bit bitIndex.  The word contains a total of nBits bits.
 *  Equivalently, return parity over all data bits whose bit-index has
 *  a 1 in the same position as in bitIndex.
 */
static int
compute_parity(HammingWord word, int bitIndex, unsigned nBits)
{
  assert(bitIndex > 0);
    if(is_parity_position(bitIndex)){
      int numBitsToPar = 0;
      unsigned bitVals[57];
      for(unsigned i=0u;i<nBits;i++){
        if(!(is_parity_position(i))&&i!=bitIndex){
          numBitsToPar++;
          bitVals[i]= word >>i;
        }else{
          bitVals[i] = 0u;
        }
        for(unsigned i=0u; i<nBits; i++){

        }
      }
  return 0;
}

/** Encode data using nParityBits Hamming code parity bits.
 *  Assumes data is within range of values which can be encoded using
 *  nParityBits.
 */
HammingWord
hamming_encode(HammingWord data, unsigned nParityBits)
{
  //@TODO
  return 0;
}

/** Decode encoded using nParityBits Hamming code parity bits.
 *  Set *hasError if an error was corrected.
 *  Assumes that data is within range of values which can be decoded
 *  using nParityBits.
 */
HammingWord
hamming_decode(HammingWord encoded, unsigned nParityBits,int *hasError)
{
  //@TODO
  return 0;
}