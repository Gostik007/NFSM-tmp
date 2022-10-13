#include "FSM.h"

bool FSM::HasState(const MachineKind& machine_kind) {
  switch(machine_kind) {
    case FSM::None: return false;
    case FSM::NFSM: return true;
    case FSM::DFSM: return TestDFSM();
    case FSM::CDFSM: return TestCDFSM();
    case FSM::MCDFSM: return TestMCDFSM();
  }
  return false;
}

// todo: add test algorithms

bool FSM::TestDFSM() {
  return true;
}

bool FSM::TestCDFSM() {
  return true;
}

bool FSM::TestMCDFSM() {
  return true;
}
