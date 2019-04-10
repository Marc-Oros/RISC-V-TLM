#ifndef FETCH_H
#define FETCH_H

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

    bool run(Registers *bank);

    uint32_t getInstr();

    private:
    uint32_t INSTR;
};

#endif