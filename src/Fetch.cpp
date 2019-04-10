#include "Fetch.h"
SC_HAS_PROCESS(Fetch);
Fetch::Fetch(sc_module_name name): sc_module(name)
, instr_bus("instr_bus")
{

}

Fetch::~Fetch()
{

}

bool Fetch::run(Registers *bank)
{
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = SC_ZERO_TIME;

    trans->set_command( tlm::TLM_READ_COMMAND );
    trans->set_data_ptr( reinterpret_cast<unsigned char*>(&INSTR) );
    trans->set_data_length( 4 );
    trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
    trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
    trans->set_dmi_allowed( false ); // Mandatory initial value
    trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

    //Read PC and get instruction
    //PC per bus?
    trans->set_address( bank->getPC() );
    instr_bus->b_transport( *trans, delay);
    return trans->is_response_error();
}

uint32_t Fetch::getInstr()
{
    return INSTR;
}