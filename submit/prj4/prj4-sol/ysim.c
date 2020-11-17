#include "ysim.h"

#include "errors.h"

/************************** Utility Routines ****************************/

/** Return nybble from op (pos 0: least-significant; pos 1:
 *  most-significant)
 */
static Byte
get_nybble(Byte op, int pos) {
  return (op >> (pos * 4)) & 0xF;
}

/************************** Condition Codes ****************************/

/** Conditions used in instructions */
typedef enum {
  ALWAYS_COND, LE_COND, LT_COND, EQ_COND, NE_COND, GE_COND, GT_COND
} Condition;

/** accessing condition code flags */
static inline bool get_cc_flag(Byte cc, unsigned flagBitIndex) {
  return !!(cc & (1 << flagBitIndex)); //wtf is with the !! bro
}
static inline bool get_zf(Byte cc) { return get_cc_flag(cc, ZF_CC); }
static inline bool get_sf(Byte cc) { return get_cc_flag(cc, SF_CC); }
static inline bool get_of(Byte cc) { return get_cc_flag(cc, OF_CC); }

/** Return true iff the condition specified in the least-significant
 *  nybble of op holds in y86.  Encoding of Figure 3.15 of Bryant's
 *  CompSys3e.
 */
bool
check_cc(const Y86 *y86, Byte op)
{
  bool ret = false;
  Condition condition = get_nybble(op, 0);
  Byte cc = read_cc_y86(y86);
  switch (condition) {
  case ALWAYS_COND:
    ret = true;
    break;
  case LE_COND:
    ret = (get_sf(cc) ^ get_of(cc)) | get_zf(cc);
    break;
  case LT_COND:
    ret = get_sf(cc) ^ get_of(cc);
    break;
  case EQ_COND:
    ret = (get_sf(cc) == get_of(cc);
    break;
  case NE_COND:
    ret = (get_sf(cc) != get_of(cc);
    break;
  case GE_COND:
    ret = (get_of(cc) ^ get_sf(cc)) | get_zf(cc);
    break;
  case GT_COND:
    ret = get_of(cc) ^ get_sf(cc);
    break;
  default: {
    Address pc = read_pc_y86(y86);
    fatal("%08lx: bad condition code %d\n", pc, condition);
    break;
    }
  }
  return ret;
}

/** return true iff word has its sign bit set */
static inline bool
isLt0(Word word) {
  return (word & (1UL << (sizeof(Word)*CHAR_BIT - 1))) != 0;
}

/** Set condition codes for addition operation with operands opA, opB
 *  and result with result == opA + opB.
 */
static void
set_add_arith_cc(Y86 *y86, Word opA, Word opB, Word result)
{
  if(result<0){
    //set SF
  }else if(result == 0){
    //set ZF 
  }else if((opA>0&&opB>0&&result<0)||(opA<0&&opB<0&&result>0)){
    //set OF
  }
}

/** Set condition codes for subtraction operation with operands opA, opB
 *  and result with result == opA - opB.
 */
static void
set_sub_arith_cc(Y86 *y86, Word opA, Word opB, Word result)
{
  if(result<0){
    //set SF
  }else if(result == 0){
    //set ZF 
  }else if((opA>0&&opB<0&&result>0)||(opA<0&&opB>0&&result<0)){
    //set OF
  }
}

static void
set_logic_op_cc(Y86 *y86, Word result)
{
  Byte cc = read_cc_y86(y86);
  result = opA + opB;
  if(result<0){
    //set SF
  }else if(result == 0){
    //set ZF 
  }
  }
}

/**************************** Operations *******************************/

static void
op1(Y86 *y86, Byte op, Register regA, Register regB)
{
  enum {ADDL_FN, SUBL_FN, ANDL_FN, XORL_FN };
  switch(op){
  case ADDL_FN:
    Register result = regA + regB;
    set_add_arith_cc(y86, regA, regB, result);  
    regA = result;
  break;
  case SUBL_FN:
    Register result = regA - regB;
    set_sub_arith_cc(y86, regA, regB, result);  
    regA = result;
  break;
  case ANDL_FN:
    Register result = regA & regB;
    set_logic_arith_cc(y86, regA, regB, result);  
    regA = result;
  break;
  case XORL_FN:
    Register result = regA ^ regB;
    set_add_arith_cc(y86, regA, regB, result); 
    regA = result; 
  break;
  }
  
}

/*********************** Single Instruction Step ***********************/

typedef enum {
  HALT_CODE, NOP_CODE, CMOVxx_CODE, IRMOVQ_CODE, RMMOVQ_CODE, MRMOVQ_CODE,
  OP1_CODE, Jxx_CODE, CALL_CODE, RET_CODE,
  PUSHQ_CODE, POPQ_CODE } BaseOpCode;

/** Execute the next instruction of y86. Must change status of
 *  y86 to STATUS_HLT on halt, STATUS_ADR or STATUS_INS on
 *  bad address or instruction.
 */
void
step_ysim(Y86 *y86)
{
  if(read_status(y86)!= STATUS_AOK){
    return;
  }
  Byte BaseOpCode = get_nybble(
  switch(
}
