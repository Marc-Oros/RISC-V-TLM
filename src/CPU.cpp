#include "CPU.h"

SC_HAS_PROCESS(CPU);
CPU::CPU(sc_module_name name, uint32_t PC): sc_module(name)
//, instr_bus("instr_bus")
{
   register_bank = new Registers();
   exec = new Execute("Execute", register_bank);
   perf = Performance::getInstance();
   log = Log::getInstance();
   fetch = new Fetch("Fetch");

   register_bank->setPC(PC);

   //register_bank->setValue(Registers::sp, (0xD0000 / 4) - 1);
   register_bank->setSP((0x10000000 / 4) - 1);
   //register_bank->setValue(Registers::sp, (0x10000000 / 4) - 1);

   irq_line_socket.register_b_transport(this, &CPU::call_interrupt);
   interrupt = false;

   int_cause = 0;
   irq_already_down = false;

   SC_THREAD(CPU_thread);
}

CPU::~CPU() {
  cout << "*********************************************" << endl;
  register_bank->dump();
  cout << "end time: " << sc_time_stamp() << endl;
  perf->dump();
  cout << "*********************************************" << endl;
}

bool CPU::cpu_process_IRQ() {
  uint32_t csr_temp;
  uint32_t new_pc, old_pc;
  bool ret_value = false;

  if (interrupt == true) {
    csr_temp = register_bank->getCSR(CSR_MSTATUS);
    if (csr_temp & MSTATUS_MIE) {
    } else  {
      log->SC_log(Log::DEBUG)  << "interrupt delayed" << endl;
      return ret_value;
    }

    csr_temp = register_bank->getCSR(CSR_MIP);

    if ( (csr_temp & MIP_MEIP ) == 0 ) {
      csr_temp |= MIP_MEIP;  // MEIP bit in MIP register (11th bit)
      register_bank->setCSR(CSR_MIP, csr_temp);
      log->SC_log(Log::DEBUG) << "Interrupt!" << endl;

      /* updated MEPC register */
      old_pc = register_bank->getPC();
      register_bank->setCSR(CSR_MEPC, old_pc);
      log->SC_log(Log::INFO) << "Old PC Value 0x" << hex << old_pc << endl;

      /* update MCAUSE register */
      register_bank->setCSR(CSR_MCAUSE, 0x80000000);

      /* set new PC address */
      new_pc = register_bank->getCSR(CSR_MTVEC);
      //new_pc = new_pc & 0xFFFFFFFC; // last two bits always to 0
      log->SC_log(Log::DEBUG) << "NEW PC Value 0x" << hex << new_pc << endl;
      register_bank->setPC(new_pc);

      ret_value = true;
      interrupt = false;
      irq_already_down = false;
    }
  } else {
	  if (irq_already_down == false) {
		csr_temp = register_bank->getCSR(CSR_MIP);
		csr_temp &= ~MIP_MEIP;
		register_bank->setCSR(CSR_MIP, csr_temp);
		irq_already_down = true;
	  }
  }

  return ret_value;
}

void CPU::forward_step(Fetch *fetch, Execute *exec, Registers *register_bank, bool incPCby2, bool PC_not_affected)
{
  if(PC_not_affected)
  {
    register_bank->incPC(incPCby2);
  } 
  exec->setInstr(fetch->getInstr());
  extension_t ext = fetch->getExt();
  exec->setExt(ext);
  switch(ext)
  {
    case BASE_EXTENSION:
      exec->set_base_d(fetch->get_base_d());
      break;
    case C_EXTENSION:
      exec->set_c_d(fetch->get_c_d());
      break;
    case M_EXTENSION:
      exec->set_m_d(fetch->get_m_d());
      break;
    case A_EXTENSION:
      exec->set_a_d(fetch->get_a_d());
      break;
    default:
      std::cout << "Extension not implemented yet" << std::endl;
  }
  register_bank->forward();
}

/**
 * main thread for CPU simulation
 * @brief CPU main thread
 */
void CPU::CPU_thread(void) {

  /*tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
  uint32_t INSTR;
  sc_time delay = SC_ZERO_TIME;*/
  bool PC_not_affected = false;
  bool incPCby2 = false;
  //One forward-writeBack cycle because a register is written before we get here
  register_bank->forward();
  register_bank->writeBack();

  while(1) 
  {
    incPCby2 = fetch->run(register_bank, log, perf);
    //Si ha de canviar el PC, s'ha de buidar el pipeline
    PC_not_affected = exec->run();
    if(!PC_not_affected)
    {
      exec->NOP_toggle();
    }
    register_bank->writeBack();

    /* Process IRQ (if any) */
    cpu_process_IRQ();

    forward_step(fetch, exec, register_bank, incPCby2, PC_not_affected);

    /* Fixed instruction time to 10 ns (i.e. 100 MHz)*/
    sc_core::wait(10, SC_NS);
  }    
} // CPU_thread


void CPU::call_interrupt(tlm::tlm_generic_payload &trans, sc_time &delay) {
  interrupt = true;
  /* Socket caller send a cause (its id) */
  memcpy(&int_cause, trans.get_data_ptr(), sizeof(int));
}
