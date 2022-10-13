#include <iostream>
#include "src/FSM/FSM.h"

int main() {
  std::ifstream in("/home/ownstreamer/proga/C++/Finite State Machines/data/data.txt");
  FSM fsm;
  fsm.GetData(in, FSM::NFSM);
//  fsm.PullEpsilonTransitions();
  fsm.Print();
//  fsm.CreateMCDFSMFromCDFSM();
//  std::cout << "DFSM after minimisation:\n";
//  fsm.Print();


  return 0;
}
