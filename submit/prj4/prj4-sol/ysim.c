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

typedef enum{
  JMP, JLE, JL, JE, JNE, JGE, JG
} JumpType;

/** accessing condition code flags */
static inline bool get_cc_flag(Byte cc, unsigned flagBitIndex) {
  return !!(cc & (1 << flagBitIndex)); //wtf is with the !! bro
}
static Byte set_cc_flags(unsigned zf_val, unsigned sf_val, unsigned of_val){
  return ((zf_val<<ZF_CC) + (sf_val <<SF_CC) + (of_val<<OF_CC));  //This causes some (manageable) problems
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
    ret = get_zf(cc); 
    break;
  case NE_COND:
    ret = (!get_zf(cc));
    break;
  case GE_COND:
    ret = get_sf(cc) & get_of(cc);
    break;
  case GT_COND:
    ret = (get_of(cc)&get_sf(cc)) | !get_zf(cc);
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
  Byte cc = 0;
  if(result<0){
    cc = set_cc_flags(0,1,0);
  }
  if(result == 0){
    cc = set_cc_flags(1,get_sf(cc),0);
  }
  if((opA>0&&opB>0&&result<0)||(opA<0&&opB<0&&result>0)){
    cc = set_cc_flags(get_zf(cc),get_sf(cc),1);
  }
  write_cc_y86(y86, cc);
}

/** Set condition codes for subtraction operation with operands opA, opB
 *  and result with result == opA - opB.
 */
static void
set_sub_arith_cc(Y86 *y86, Word opA, Word opB, Word result)
{
  Byte cc = 0;
  if(result<0){
   cc = set_cc_flags(0,1,0);
  }
  if(result == 0){
    cc = set_cc_flags(1,get_sf(cc),0);
  }
  if((opA>0&&opB<0&&result>0)||(opA<0&&opB>0&&result<0)){
    cc = set_cc_flags(get_zf(cc),get_sf(cc),1);
  }
  write_cc_y86(y86, cc);
}

static void
set_logic_op_cc(Y86 *y86, Word result)
{
  Byte cc = 0;
  if(result<0){
    cc = set_cc_flags(0,1,0);
  }if(result == 0){
    cc = set_cc_flags(1,get_sf(cc),0);
  }
  cc = set_cc_flags(get_zf(cc),get_sf(cc),0);
  write_cc_y86(y86, cc);
}

/**************************** Operations *******************************/


static void
op1(Y86 *y86, Byte op, Register regA, Register regB)
{
  enum {ADDL_FN, SUBL_FN, ANDL_FN, XORL_FN };
  Word result = 0;
  Word valA = read_register_y86(y86, regA);
  Word valB = read_register_y86(y86, regB);
  switch(op){
  case ADDL_FN:
    result = valA + valB;
    set_add_arith_cc(y86, valA, valB, result);  
    break;
  case SUBL_FN:
    result = valB - valA; //Might be wrong order
    set_sub_arith_cc(y86, valA, valB, result); //Sign bit setting may need to be reworked.  A<-->B
    break;
  case ANDL_FN:
    result = valA & valB;
    set_logic_op_cc(y86, result);  
    regA = result;
    break;
  case XORL_FN:
    result = valA ^ valB;
    set_logic_op_cc(y86, result); 
    break;
  }
  write_register_y86(y86, regB, result);
}

static void
Jxx(Y86 *y86, Byte op, Word dest){
  if(check_cc(y86, op)){ //is it really that simple tho?
    write_pc_y86(y86, (Address) dest);
  }else{
  write_pc_y86(y86, read_pc_y86(y86)+sizeof(Byte)+sizeof(Word));
  }
}

static void
CMOVxx(Y86 *y86, Byte op){
  if(check_cc(y86, op)){
    Byte registers = read_memory_byte_y86(y86, read_pc_y86(y86)+sizeof(Byte));
    Register regA = get_nybble(registers, 1);
    Register regB = get_nybble(registers, 0);
    Word valB = read_register_y86(y86, regB);
    write_register_y86(y86, regA, valB);
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
  Byte pc = read_pc_y86(y86);
  Byte op = read_memory_byte_y86(y86, pc);//IF THAT FRICKING WORKS I'LL SHITE IN ME PANTALOON
  Byte baseOp = get_nybble(op, 1);
  
  if(read_status_y86(y86)!= STATUS_AOK){
    return;
  }
  switch(baseOp){
  case HALT_CODE: //OP 0
    write_status_y86(y86, STATUS_HLT);
    break;
  case NOP_CODE: //OP 1
    write_pc_y86(y86, pc+sizeof(Byte));
    break;
  case CMOVxx_CODE: //OP 2
    CMOVxx(y86, op);
    write_pc_y86(y86, pc + 2*sizeof(Byte));
    break;
  case IRMOVQ_CODE: ;//OP 3 - dummy line to allow declarations
    Byte reg = get_nybble(read_memory_byte_y86(y86, pc+sizeof(Byte)),0);
    Word val = read_memory_word_y86(y86, pc+(2*sizeof(Byte)));
    write_register_y86(y86, reg, val); 
    write_pc_y86(y86, pc+2*sizeof(Byte)+sizeof(Word));
    break;
  case RMMOVQ_CODE: //OP 4
    
    break;
  case MRMOVQ_CODE: //OP 5
  
    break;
  case OP1_CODE: ;//OP 6 - dummy line to allow declarations
    Byte fn = get_nybble(op, 0);                       //Get function code (when necessary)
    Register regA = get_nybble(read_memory_byte_y86(y86, pc+1),1);  //get registers (when applicable)
    Register regB = get_nybble(read_memory_byte_y86(y86, pc+1),0);  //Can't be nested in a case.
    op1(y86, fn, regA, regB);
    write_pc_y86(y86, pc + (2*sizeof(Byte)));
    break;
  case Jxx_CODE: //OP 7
    Jxx(y86, op, read_memory_word_y86(y86, pc+sizeof(Byte)));
    //Jxx writes to pc, no updating needed here.
    break;
  case CALL_CODE: //OP 8
    //stack shit
    break;
  case RET_CODE: //OP 9
    //Stack shit
    break;
  case PUSHQ_CODE: //OP A
    //stack shit
  break;
  case POPQ_CODE: //OP B
    //stack shit
    break;
  default:
    write_status_y86(y86, STATUS_INS);
    break;
  }
}
