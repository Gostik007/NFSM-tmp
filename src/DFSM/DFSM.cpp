#include "DFSM.h"

DFSM::DFSM(NFSM& nfsm) {
  CreateFrom(nfsm);
}

void DFSM::CreateFrom(NFSM& nfsm) {
  assert((void("DFSM::DFSM : nfsm.Empty()\n"), !nfsm.Empty()));

  nfsm.PullEpsilonTransitions();
  std::vector<boost::dynamic_bitset<>> tmp_states;
  std::vector<size_t> active_states;
  std::queue<boost::dynamic_bitset<>> queue;
  boost::dynamic_bitset<> curr_bitset_state(nfsm.states_.size(), 1);
//  curr_bitset_state.set(0, true); // here was 0
  boost::dynamic_bitset<> curr_transition(nfsm.states_.size(), 0);
  std::unordered_map<boost::dynamic_bitset<>, size_t> bitset_to_state_ind;
  std::vector<StateBitset> states_bitset;
  size_t curr_state_ind = 0;
  bool is_final;

  queue.push(curr_bitset_state);
  while (!queue.empty()) {
    curr_bitset_state = queue.front();
    queue.pop();
    if (bitset_to_state_ind.contains(curr_bitset_state)) {
      continue;
    }
    for (size_t i = 0; i < nfsm.states_.size(); ++i) {
      if (curr_bitset_state.test(i)) {
        active_states.push_back(i);
      }
    }
    states_bitset.push_back({is_final, {}});
    states_bitset[curr_state_ind].transitions.resize(nfsm.states_[0].transitions_.size());
    for (auto& active_state_ind : active_states) {
      states_bitset[curr_state_ind].final |= nfsm.states_[active_state_ind].final_;
    }
    for (size_t i = 0; i < nfsm.states_[0].transitions_.size(); ++i) {
      curr_transition ^= curr_transition;
      for (auto& active_state_ind : active_states) {
        curr_transition |= nfsm.states_[active_state_ind].transitions_[i];
      }
      states_bitset[curr_state_ind].transitions[i] = curr_transition;
      queue.push(curr_transition);
    }
    active_states.clear();
    bitset_to_state_ind.insert({curr_bitset_state, curr_state_ind++});
  }
  // check for existence of stock vertex and fill it with
  curr_bitset_state = boost::dynamic_bitset<>(nfsm.states_.size(), 0);
  if (bitset_to_state_ind.contains(curr_bitset_state)) {
    curr_state_ind = bitset_to_state_ind[curr_bitset_state];
    for (auto& transition : states_bitset[curr_state_ind].transitions) {
      transition = curr_bitset_state;
    }
  }
  // make 'states_'
  // btw: it makes CDFSM
  states_.resize(states_bitset.size());
  for (size_t i = 0; i < states_.size(); ++i) {
    states_[i].final = states_bitset[i].final;
    states_[i].transitions.resize(states_bitset[i].transitions.size());
    for (size_t j = 0; j < states_bitset[i].transitions.size(); ++j) {
      states_[i].transitions[j] = bitset_to_state_ind[states_bitset[i].transitions[j]];
    }
  }
}

void DFSM::Print() {
  std::cout << "--------DFSM--------\n";
  for (size_t i = 0; i < states_.size(); ++i) {
    if (states_[i].final) {
      std::cout << "STATE " << i << "*: ";
    } else {
      std::cout << "STATE " << i << ": ";
    }
    std::cout << "\n";
    for (size_t j = 0; j < states_[i].transitions.size(); ++j) {
      std::cout << "transition by " << j << " to " << states_[i].transitions[j] << "\n";
    }
    std::cout << "\n";
  }
}

size_t DFSM::GetAlphabetSize() {
  if (Empty()) {
    return 0;
  }
  return states_[0].transitions.size();
}

FSM DFSM::GetFSM(const std::vector<SymbolType>& alphabet_,
                 const std::unordered_map<SymbolType, size_t>& letter_to_num) {
  /// assuming that we have CDFSM
  FSM fsm;
  fsm.current_machine_kind_ = FSM::DFSM;
  fsm.states_.resize(Size());
  fsm.alphabet_ = alphabet_;
  fsm.letter_to_num_ = letter_to_num;
  for (size_t i = 0; i < Size(); ++i) {
    fsm.states_[i] = states_[i].final;
    for (size_t letter_ind = 0; letter_ind < states_[i].transitions.size(); ++letter_ind) {
      if (states_[i][letter_ind] != -1) {
        fsm.transitions_.emplace_back(i, letter_ind + 1, states_[i][letter_ind]);
      }
    }
  }
  return fsm;
}

void FSM::CreateDFSM() {
  PullEpsilonTransitions();
  class NFSM nfsm(*this);
  class DFSM dfsm(nfsm);
  *this = dfsm.GetFSM(alphabet_, letter_to_num_);
  current_machine_kind_ = DFSM;
}

void FSM::CreateCDFSM() {
  CreateDFSM();
  current_machine_kind_ = CDFSM;
}

void FSM::CreateAdditionToCDFSM() {
//  CreateCDFSM();
//  class NFSM nfsm(*this);
//  class DFSM dfsm(nfsm);

  for (size_t i = 0; i < Size(); ++i) {
    states_[i] = ~states_[i];
  }
}

void FSM::CreateMCDFSMFromCDFSM() {
  class NFSM nfsm(*this);
  class DFSM dfsm(nfsm);
  dfsm.Minimize();
  *this = dfsm.GetFSM(alphabet_, letter_to_num_);
  current_machine_kind_ = MCDFSM;
}

bool FSM::DFSMIsAccepted(const std::vector<size_t>& word) {


  return false;
}

bool FSM::CDFSMIsAccepted(const std::vector<size_t>& word) {


  return false;
}