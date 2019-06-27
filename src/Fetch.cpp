#include "Fetch.h"
SC_HAS_PROCESS(Fetch);
Fetch::Fetch(sc_module_name name): sc_module(name)
, instr_bus("instr_bus")
{

}

Fetch::~Fetch()
{

}

op_C_Codes Fetch::decode_c_instruction(Instruction &inst) {

  C_Instruction c_inst(inst.getInstr());

  return c_inst.decode();
}

op_M_Codes Fetch::decode_m_instruction(Instruction &inst) {
  M_Instruction m_inst(inst.getInstr());

  return m_inst.decode();
}


op_A_Codes Fetch::decode_a_instruction(Instruction inst) {
  A_Instruction a_inst(inst.getInstr());

  return a_inst.decode();
}

opCodes Fetch::decode_base_instruction(Instruction &inst) {
  return inst.decode();
}

bool Fetch::run(Registers *register_bank, Log *log, Performance *perf)
{
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = SC_ZERO_TIME;
    bool incPCby2;
    uint32_t INSTR;

    trans->set_command( tlm::TLM_READ_COMMAND );
    trans->set_data_ptr( reinterpret_cast<unsigned char*>(&INSTR) );
    trans->set_data_length( 4 );
    trans->set_streaming_width( 4 ); // = data_length to indicate no streaming
    trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
    trans->set_dmi_allowed( false ); // Mandatory initial value
    trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

    //Read PC and get instruction
    trans->set_address( register_bank->getPC() );
    instr_bus->b_transport( *trans, delay);
    if ( trans->is_response_error() ) {
        SC_REPORT_ERROR("CPU base", "Read memory");
        return trans->is_response_error();
      } else {
        perf->codeMemoryRead();
        //Com generar una instruccio nova amb mem estatica?
        Instruction tmp(INSTR);
        inst = tmp;
        switch(inst.check_extension()) {
          case BASE_EXTENSION:
            extension = BASE_EXTENSION;
            base_inst_d = decode_base_instruction(inst);
            incPCby2 = false;
            break;
          case C_EXTENSION:
            extension = C_EXTENSION;
            c_inst_d = decode_c_instruction(inst);
            incPCby2 = true;
            break;
          case M_EXTENSION:
            extension = M_EXTENSION;
            m_inst_d = decode_m_instruction(inst);
            incPCby2 = false;
            break;
          case A_EXTENSION:
            extension = A_EXTENSION;
            a_inst_d = decode_a_instruction(inst);
            incPCby2 = false;
            break;
          default:
            std::cout << "Extension not implemented yet" << std::endl;
            inst.dump();
            //exec->NOP(inst);
          }
        //register_bank->incPC(incPCby2);
        //return trans->is_response_error();
        return incPCby2;
      }
}

Instruction Fetch::getInstr()
{
  return inst;
}

extension_t Fetch::getExt()
{
  return extension;
}

opCodes Fetch::get_base_d()
{
  return base_inst_d;
}

op_C_Codes Fetch::get_c_d()
{
  return c_inst_d;
}

op_M_Codes Fetch::get_m_d()
{
  return m_inst_d;
}

op_A_Codes Fetch::get_a_d()
{
  return a_inst_d;
}