/*!
   \file CPU.h
   \brief Main CPU class
   \author Màrius Montón
   \date August 2018
*/
#ifndef CPU_BASE_H
#define CPU_BASE_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "memory.h"
#include "Execute.h"
#include "Registers.h"
#include "Log.h"
#include "Instruction.h"
#include "C_Instruction.h"
#include "M_Instruction.h"
#include "A_Instruction.h"
#include "Fetch.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

/**
 * @brief RISC_V CPU model
 * @param name name of the module
 */
class CPU: sc_module {
public:

  /**
   * @brief Instruction Memory bus socket
   * @param trans transction to perfoem
   * @param delay time to annotate
   */
  //  tlm_utils::simple_initiator_socket<CPU> instr_bus;

  /**
   * @brief IRQ line socket
   * @param trans transction to perform (empty)
   * @param delay time to annotate
   */
  tlm_utils::simple_target_socket<CPU> irq_line_socket;

  /**
   * @brief Constructor
   * @param name Module name
   * @param PC   Program Counter initialize value
   */
  CPU(sc_module_name name, uint32_t PC);

  /**
   * @brief Destructor
   */
  ~CPU();

  Execute *exec;

  Fetch *fetch;

private:
  Registers *register_bank;
  Performance *perf;
  Log *log;

  bool interrupt;
  uint32_t int_cause;
  bool irq_already_down;
  /**
   *
   * @brief Process and triggers IRQ if all conditions met
   * @return true if IRQ is triggered, false otherwise
   */
  bool cpu_process_IRQ();

  /**
   * @brief Executes default ISA instruction
   * @param  inst instruction to execute
   * @return  true if PC is affected by instruction
   */
  bool process_base_instruction(Instruction &inst);

  bool process_c_instruction(Instruction &inst);

  bool process_m_instruction(Instruction &inst);

  bool process_a_instruction(Instruction inst);

  /**
   * @brief shifts data forward at the end of the cycle
   * @params  stage objects to interact with them
   * @return  nothing
   */

  void forward_step(Fetch *Fetch, Execute *exec);

  void CPU_thread(void);

  /**
   * @brief callback for IRQ simple socket
   * @param trans transaction to perform (empty)
   * @param delay time to annotate
   *
   * When called it triggers an IRQ
   */
  void call_interrupt(tlm::tlm_generic_payload &trans, sc_time &delay);
};

#endif
