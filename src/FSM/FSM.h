#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <cassert>

using SymbolType = char;

class NFSM;
class DFSM;

// single-letter finite state machine
struct FSM {
  enum MachineKind {
    None,
    NFSM,
    DFSM,
    CDFSM,
    MCDFSM
  };
  struct Transition;
  // size_t - number of state, bool - final state
  std::vector<bool> states_;
  std::vector<SymbolType> alphabet_;
  std::vector<Transition> transitions_;
  std::unordered_map<SymbolType, size_t> letter_to_num_;
  MachineKind current_machine_kind_ = None;

 private:
  bool HasState(const MachineKind& machine_kind);
  bool TestDFSM();
  bool TestCDFSM();
  bool TestMCDFSM();

  bool NFSMIsAccepted(const std::vector<size_t>& word);
  bool DFSMIsAccepted(const std::vector<size_t>& word);
  bool CDFSMIsAccepted(const std::vector<size_t>& word);

  void SortTransitions();

 public:
  FSM() = default;
  template <typename InputType>
  void GetData(InputType& input_stream, const MachineKind& machine_kind);
  void Print();

  void PullEpsilonTransitions();
  void CreateDFSM();
  void CreateCDFSM();
  void CreateAdditionToCDFSM();
  void CreateMCDFSMFromCDFSM();
  void CreateRegularFromCDFSM();

  bool IsAccepted(const std::string& word);

  size_t Size() { return states_.size(); }
};

struct FSM::Transition {
  size_t curr_state;
  size_t letter_ind;
  size_t next_state;
  Transition() = default;
  Transition(size_t curr_state, size_t letter_ind, size_t next_state) :
  curr_state(curr_state), letter_ind(letter_ind), next_state(next_state) {}
  size_t& operator[](size_t i) {
    switch (i) {
      case 0: return curr_state;
      case 1: return letter_ind;
      case 2: return next_state;
      default:
        std::cerr << "FSM::Transition::operator[] : index out of bounds\n";
        exit(3);
    }
  }
  size_t operator[](size_t i) const {
    switch (i) {
      case 0: return curr_state;
      case 1: return letter_ind;
      case 2: return next_state;
      default:
        std::cerr << "FSM::Transition::operator[] : index out of bounds\n";
        exit(3);
    }
  }
};

template <typename InputType>
void FSM::GetData(InputType& input_stream, const MachineKind& machine_kind) {
  // entering states (numeration from 0)
  size_t num_of_states;
  input_stream >> num_of_states;
  states_.resize(num_of_states);
  std::string curr_state;
  for (auto&& state : states_) {
    input_stream >> curr_state;
    assert((void("FSM::GetData : curr_state.empty()\n"), !curr_state.empty()));
    // todo: change behaviour input_stream case of states started not from 0
    if (curr_state.back() == '*') {
      state = true;
      //      curr_state.erase(curr_state.end() - 1); // needed in case of states started not from 0
    } else {
      state = false;
    }
  }
  // entering alphabet (0 must be epsilon transition designation)
  size_t alphabet_size;
  input_stream >> alphabet_size;
  alphabet_.resize(alphabet_size);
  for (size_t i = 0; i < alphabet_.size(); ++i) {
    input_stream >> alphabet_[i];
    letter_to_num_.emplace(alphabet_[i], i);
  }
  // entering transitions
  size_t num_of_transitions;
  input_stream >> num_of_transitions;
  transitions_.resize(num_of_transitions);
  char curr_letter;
  for (auto&& transition : transitions_) {
    input_stream >> transition.curr_state >> curr_letter >> transition.next_state;
    transition.letter_ind = letter_to_num_[curr_letter];
  }

  assert((void("FSM::GetData : incorrect machine_kind"), HasState(machine_kind)));
  current_machine_kind_ = machine_kind;
}

template <>
void FSM::GetData(std::istream& input_stream, const MachineKind& machine_kind) {
  // entering states (numeration from 0)
  std::cout << "Enter number of states: " << std::flush;
  size_t num_of_states;
  input_stream >> num_of_states;
  states_.resize(num_of_states);
  std::cout << "Enter states: " << std::flush;
  std::string curr_state;
  for (auto&& state : states_) {
    input_stream >> curr_state;
    assert((void("FSM::GetData : curr_state.empty()\n"), !curr_state.empty()));
    // todo: change behaviour input_stream case of states started not from 0
    if (curr_state.back() == '*') {
      state = true;
      //      curr_state.erase(curr_state.end() - 1); // needed in case of states started not from 0
    } else {
      state = false;
    }
  }
  // entering alphabet (0 must be epsilon transition designation)
  std::cout << "Enter alphabet size: " << std::flush;
  size_t alphabet_size;
  input_stream >> alphabet_size;
  alphabet_.resize(alphabet_size);
  std::cout << "Enter alphabet: " << std::flush;
  for (size_t i = 0; i < alphabet_.size(); ++i) {
    input_stream >> alphabet_[i];
    letter_to_num_.emplace(alphabet_[i], i);
  }
  // entering transitions
  size_t num_of_transitions;
  std::cout << "Enter number of transitions: " << std::flush;
  input_stream >> num_of_transitions;
  transitions_.resize(num_of_transitions);
  std::cout << "Enter transitions: " << std::flush;
  char curr_letter;
  for (auto&& transition : transitions_) {
    input_stream >> transition.curr_state >> curr_letter >> transition.next_state;
    transition.letter_ind = letter_to_num_[curr_letter];
  }

  assert((void("FSM::GetData : incorrect machine_kind"), HasState(machine_kind)));
  current_machine_kind_ = machine_kind;
}

/* example
  3
  0 1* 2*
  3
  e a b
  3
  0 a 1
  0 b 2
  1 e 2
 */