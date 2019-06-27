/*!
   \file Execute.h
   \brief RISC-V ISA implementation
   \author Màrius Montón
   \date August 2018
*/
#ifndef Execute_H
#define Execute_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
#include <set>
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "memory.h"
#include "Instruction.h"
#include "C_Instruction.h"
#include "M_Instruction.h"
#include "A_Instruction.h"
#include "Registers.h"
#include "Log.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

/**
 * @brief Risc_V execute module
 */
class Execute : sc_module {
public:

  /**
   * @brief Constructor
   * @param name          module name
   * @param register_bank pointer to register bank to use
   */
  Execute(sc_module_name name,
    Registers *register_bank);

  /* Quick & dirty way to publish a socket though modules */
  tlm_utils::simple_initiator_socket<Execute> data_bus;

  bool run();

  void NOP_toggle();

  /*
  * Setters for the execute stage
  * p_indicates that the item comes from the previous stage
  */

  void setInstr(Instruction p_inst);
  void setExt(extension_t p_extension);
  void set_base_d(opCodes p_base_inst_d);
  void set_c_d(op_C_Codes p_c_inst_d);
  void set_m_d(op_M_Codes p_m_inst_d);
  void set_a_d(op_A_Codes p_a_inst_d);

  bool process_base_instruction();

  bool process_c_instruction();

  bool process_m_instruction();

  bool process_a_instruction();

  bool LUI();
  bool AUIPC();

  bool JAL(bool c_extension = false, int m_rd = 1);
  bool JALR(bool c_extension = false);

  bool BEQ();
  bool BNE();
  bool BLT();
  bool BGE();
  bool BLTU();
  bool BGEU();

  bool LB();
  bool LH();
  bool LW(bool c_extension = false);
  bool LBU();
  bool LHU();

  bool SB();
  bool SH();
  bool SW(bool c_extension = false);
  bool SBU();
  bool SHU();

  bool ADDI(bool c_extension = false);
  bool SLTI();
  bool SLTIU();
  bool XORI();
  bool ORI();
  bool ANDI();
  bool SLLI();
  bool SRLI();
  bool SRAI();

  bool ADD();
  bool SUB();
  bool SLL();
  bool SLT();
  bool SLTU();

  bool XOR();
  bool SRL();
  bool SRA();
  bool OR();
  bool AND();

  bool FENCE();
  bool ECALL();
  bool EBREAK();

  bool CSRRW();
  bool CSRRS();
  bool CSRRC();
  bool CSRRWI();
  bool CSRRSI();
  bool CSRRCI();

/*********************** Privileged Instructions ******************************/
  bool MRET();
  bool SRET();
  bool WFI();
  bool SFENCE();
  
  /* C Extensions */
  bool C_JR();
  bool C_MV();
  bool C_LWSP();
  bool C_ADDI4SPN();
  bool C_SLLI();
  bool C_ADDI16SP();
  bool C_SWSP();
  bool C_BEQZ();
  bool C_BNEZ();
  bool C_LI();
  bool C_SRLI();
  bool C_SRAI();
  bool C_ANDI();
  bool C_ADD();
  bool C_SUB();
  bool C_XOR();
  bool C_OR();
  bool C_AND();

  /* M Extensinos */
  bool M_MUL();
  bool M_MULH();
  bool M_MULHSU();
  bool M_MULHU();
  bool M_DIV();
  bool M_DIVU();
  bool M_REM();
  bool M_REMU();

  /* A Extensinos */
  bool A_LR();
  bool A_SC();
  bool A_AMOSWAP();
  bool A_AMOADD();
  bool A_AMOXOR();
  bool A_AMOAND();
  bool A_AMOOR();
  bool A_AMOMIN();
  bool A_AMOMAX();
  bool A_AMOMINU();
  bool A_AMOMAXU();

  bool NOP();

private:
  uint32_t readDataMem(uint32_t addr, int size);
  void writeDataMem(uint32_t addr, uint32_t data, int size);

  void RaiseException(uint32_t cause, uint32_t inst = 0);

  std::set<uint32_t> TLB_A_Entries;

  void TLB_reserve(uint32_t address);
  bool TLB_reserved(uint32_t address);

  Registers *regs;
  Performance *perf;
  Log *log;

  //Donar valor o crear constructor buit
  Instruction inst;
  //opCode generat, hi ha 4 possibilitats
  extension_t extension;
  opCodes base_inst_d;
  op_C_Codes c_inst_d;
  op_M_Codes m_inst_d;
  op_A_Codes a_inst_d;
  bool stage_NOP = true;
};


#endif
