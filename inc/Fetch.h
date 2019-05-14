#ifndef FETCH_H
#define FETCH_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include "memory.h"
#include "Registers.h"
#include "Log.h"
#include "Instruction.h"
#include "C_Instruction.h"
#include "M_Instruction.h"
#include "A_Instruction.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

class Fetch: sc_module {
    public:
    tlm_utils::simple_initiator_socket<Fetch> instr_bus;
    /**
     * @brief Constructor
     * @param name Module name
     */
    Fetch(sc_module_name name);

    /**
     * @brief Destructor
     */
    ~Fetch();

    opCodes decode_base_instruction(Instruction &inst);

    op_C_Codes decode_c_instruction(Instruction &inst);

    op_M_Codes decode_m_instruction(Instruction &inst);

    op_A_Codes decode_a_instruction(Instruction inst);

    bool run(Registers *bank, Log *log);

    Instruction getInstr();
    extension_t getExt();
    opCodes get_base_d();
    op_C_Codes get_c_d();
    op_M_Codes get_m_d();
    op_A_Codes get_a_d();

    private:
    //Donar valor o crear constructor buit
    Instruction inst;
    //opCode generat, hi ha 4 possibilitats
    extension_t extension;
    opCodes base_inst_d;
    op_C_Codes c_inst_d;
    op_M_Codes m_inst_d;
    op_A_Codes a_inst_d;
};

#endif