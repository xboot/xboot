#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

#include <configs.h>
#include <default.h>

enum arm_instruction_type
{
	ARM_UNKNOWN_INSTUCTION,

	/* branch instructions */
	ARM_B,
	ARM_BL,
	ARM_BX,
	ARM_BLX,

	/* data processing instructions */
	ARM_AND,
	ARM_EOR,
	ARM_SUB,
	ARM_RSB,
	ARM_ADD,
	ARM_ADC,
	ARM_SBC,
	ARM_RSC,
	ARM_TST,
	ARM_TEQ,
	ARM_CMP,
	ARM_CMN,
	ARM_ORR,
	ARM_MOV,
	ARM_BIC,
	ARM_MVN,

	/* load / store instructions */
	ARM_LDR,
	ARM_LDRB,
	ARM_LDRT,
	ARM_LDRBT,

	ARM_LDRH,
	ARM_LDRSB,
	ARM_LDRSH,

	ARM_LDM,

	ARM_STR,
	ARM_STRB,
	ARM_STRT,
	ARM_STRBT,

	ARM_STRH,

	ARM_STM,

	/* status register access instructions */
	ARM_MRS,
	ARM_MSR,

	/* multiply instructions */
	ARM_MUL,
	ARM_MLA,
	ARM_SMULL,
	ARM_SMLAL,
	ARM_UMULL,
	ARM_UMLAL,

	/* miscellaneous instructions */
	ARM_CLZ,

	/* exception generating instructions */
	ARM_BKPT,
	ARM_SWI,

	/* coprocessor instructions */
	ARM_CDP,
	ARM_LDC,
	ARM_STC,
	ARM_MCR,
	ARM_MRC,

	/* semaphore instructions */
	ARM_SWP,
	ARM_SWPB,

	/* enhanced dsp extensions */
	ARM_MCRR,
	ARM_MRRC,
	ARM_PLD,
	ARM_QADD,
	ARM_QDADD,
	ARM_QSUB,
	ARM_QDSUB,
	ARM_SMLAxy,
	ARM_SMLALxy,
	ARM_SMLAWy,
	ARM_SMULxy,
	ARM_SMULWy,
	ARM_LDRD,
	ARM_STRD,

	ARM_UNDEFINED_INSTRUCTION = 0xffffffff,
};

struct arm_b_bl_bx_blx_instr
{
	x_s32 reg_operand;
	x_u32 target_address;
};

union arm_shifter_operand
{
	struct {
		x_u32 immediate;
	} immediate;
	struct {
		x_u8 Rm;
		x_u8 shift;			/* 0: LSL, 1: LSR, 2: ASR, 3: ROR, 4: RRX */
		x_u8 shift_imm;
	} immediate_shift;
	struct {
		x_u8 Rm;
		x_u8 shift;
		x_u8 Rs;
	} register_shift;
};

struct arm_data_proc_instr
{
	x_s32 variant;			/* 0: immediate, 1: immediate_shift, 2: register_shift */
	x_u8 S;
	x_u8 Rn;
	x_u8 Rd;
	union arm_shifter_operand shifter_operand;
};

struct arm_load_store_instr
{
	x_u8 Rd;
	x_u8 Rn;
	x_u8 U;
	x_s32 index_mode;		/* 0: offset, 1: pre-indexed, 2: post-indexed */
	x_s32 offset_mode;		/* 0: immediate, 1: (scaled) register */
	union
	{
		x_u32 offset;
		struct {
			x_u8 Rm;
			x_u8 shift; 	/* 0: LSL, 1: LSR, 2: ASR, 3: ROR, 4: RRX */
			x_u8 shift_imm;
		} reg;
	} offset;
};

struct arm_load_store_multiple_instr
{
	x_u8 Rn;
	x_u32 register_list;
	x_u8 addressing_mode; 	/* 0: IA, 1: IB, 2: DA, 3: DB */
	x_u8 S;
	x_u8 W;
};

struct arm_instruction
{
	enum arm_instruction_type type;
	x_s8 text[128];
	x_u32 opcode;
	x_u32 instruction_size;

	union {
		struct arm_b_bl_bx_blx_instr b_bl_bx_blx;
		struct arm_data_proc_instr data_proc;
		struct arm_load_store_instr load_store;
		struct arm_load_store_multiple_instr load_store_multiple;
	} info;
};


x_s32 arm_evaluate_opcode(x_u32 opcode, x_u32 address, struct arm_instruction *instruction);
x_s32 thumb_evaluate_opcode(x_u16 opcode, x_u32 address, struct arm_instruction *instruction);

#endif /* __DISASSEMBLER_H__ */
