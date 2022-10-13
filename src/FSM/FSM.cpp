#include "FSM.h"

//template <typename InputType>
//void FSM::GetData(InputType& input_stream, const MachineKind& machine_kind) {
//  // entering states (numeration from 0)
//  size_t num_of_states;
//  input_stream >> num_of_states;
//  states_.resize(num_of_states);
//  std::string curr_state;
//  for (auto&& state : states_) {
//    input_stream >> curr_state;
//    assert((void("FSM::GetData : curr_state.empty()\n"), !curr_state.empty()));
//    // todo: change behaviour input_stream case of states started not from 0
//    if (curr_state.back() == '*') {
//      state = true;
////      curr_state.erase(curr_state.end() - 1); // needed in case of states started not from 0
//    } else {
//      state = false;
//    }
//  }
//  // entering alphabet (0 must be epsilon transition designation)
//  size_t alphabet_size;
//  input_stream >> alphabet_size;
//  alphabet_.resize(alphabet_size);
//  for (size_t i = 0; i < alphabet_.size(); ++i) {
//    input_stream >> alphabet_[i];
//    letter_to_num_.emplace(alphabet_[i], i);
//  }
//  // entering transitions
//  size_t num_of_transitions;
//  input_stream >> num_of_transitions;
//  transitions_.resize(num_of_transitions);
//  char curr_letter;
//  for (auto&& transition : transitions_) {
//    input_stream >> transition.curr_state >> curr_letter >> transition.next_state;
//    transition.letter_ind = letter_to_num_[curr_letter];
//  }
//
//  assert((void("FSM::GetData : incorrect machine_kind"), HasState(machine_kind)));
//  current_machine_kind_ = machine_kind;
//}
//
//template <>
//void FSM::GetData(std::istream& input_stream, const MachineKind& machine_kind) {
//  // entering states (numeration from 0)
//  std::cout << "Enter number of states: " << std::flush;
//  size_t num_of_states;
//  input_stream >> num_of_states;
//  states_.resize(num_of_states);
//  std::cout << "Enter states: " << std::flush;
//  std::string curr_state;
//  for (auto&& state : states_) {
//    input_stream >> curr_state;
//    assert((void("FSM::GetData : curr_state.empty()\n"), !curr_state.empty()));
//    // todo: change behaviour input_stream case of states started not from 0
//    if (curr_state.back() == '*') {
//      state = true;
//      //      curr_state.erase(curr_state.end() - 1); // needed in case of states started not from 0
//    } else {
//      state = false;
//    }
//  }
//  // entering alphabet (0 must be epsilon transition designation)
//  std::cout << "Enter alphabet size: " << std::flush;
//  size_t alphabet_size;
//  input_stream >> alphabet_size;
//  alphabet_.resize(alphabet_size);
//  std::cout << "Enter alphabet: " << std::flush;
//  for (size_t i = 0; i < alphabet_.size(); ++i) {
//    input_stream >> alphabet_[i];
//    letter_to_num_.emplace(alphabet_[i], i);
//  }
//  // entering transitions
//  size_t num_of_transitions;
//  std::cout << "Enter number of transitions: " << std::flush;
//  input_stream >> num_of_transitions;
//  transitions_.resize(num_of_transitions);
//  std::cout << "Enter transitions: " << std::flush;
//  char curr_letter;
//  for (auto&& transition : transitions_) {
//    input_stream >> transition.curr_state >> curr_letter >> transition.next_state;
//    transition.letter_ind = letter_to_num_[curr_letter];
//  }
//
//  assert((void("FSM::GetData : incorrect machine_kind"), HasState(machine_kind)));
//  current_machine_kind_ = machine_kind;
//}

//void FSM::GetData(const MachineKind& machine_kind,
//                  std::basic_istream<char>& input_stream, bool is_cin) {
//  current_machine_kind_ = machine_kind;
//  // numeration from 0
//  size_t num_of_states;
//  if (is_cin) {
//    std::cout << "Enter number of states: ";
//  }
//  input_stream >> num_of_states;
//  states_.resize(num_of_states);
//  std::string curr_state;
//  if (is_cin) {
//    std::cout << "Enter states: ";
//  }
//  for (size_t i = 0; i < states_.size(); ++i) {
//    input_stream >> curr_state;
//    if (curr_state.empty()) {
//      std::cerr << "FSM::FSM() : curr_state.empty()\n";
//      exit(2);
//    }
//    // todo: change behaviour input_stream case of states started not from 0
//    if (curr_state.back() == '*') {
//      states_[i] = true;
//      curr_state.erase(curr_state.end() - 1);
//    } else {
//      states_[i] = false;
//    }
//  }
//  size_t alphabet_size;
//  if (is_cin) {
//    std::cout << "Enter alphabet size: " << std::flush;
//  }
//  input_stream >> alphabet_size;
//  alphabet_.resize(alphabet_size);
//  if (is_cin) {
//    std::cout << "Enter alphabet: " << std::flush;
//  }
//  // 0 must be epsilon transition designation
//  for (size_t i = 0; i < alphabet_.size(); ++i) {
//    input_stream >> alphabet_[i];
//    letter_to_num_.emplace(alphabet_[i], i);
//  }
//  size_t num_of_transitions;
//  if (is_cin) {
//    std::cout << "Enter number of transitions: ";
//  }
//  input_stream >> num_of_transitions;
//  transitions_.resize(num_of_transitions);
//  std::string curr_letter;
//  if (is_cin) {
//    std::cout << "Enter transitions: " << std::flush;
//  }
//  for (auto& transition : transitions_) {
//    input_stream >> transition.curr_state >> curr_letter >> transition.next_state;
//    transition.letter_ind = letter_to_num_[curr_letter];
//  }
//}

bool FSM::IsAccepted(const std::string& word) {
  std::vector<size_t> word_in_nums;
  word_in_nums.reserve(word.size());
  word_in_nums.push_back(letter_to_num_[word[0]]);
  for (const auto& letter : word) {
    word_in_nums.push_back(letter_to_num_[letter]);
  }
  switch(current_machine_kind_) {
    case None: return false;
    case NFSM: return NFSMIsAccepted(word_in_nums);
    case DFSM: return DFSMIsAccepted(word_in_nums);
    case CDFSM:
    case MCDFSM: return CDFSMIsAccepted(word_in_nums);
  }
  return false;
}

void FSM::Print() {
  assert((void("FSM::Print : !states_.empty()"), !states_.empty()));
  SortTransitions();
  std::cout << "--------FSM--------\n";
  std::cout << "Number of states: " << states_.size() << "\n";
  std::cout << "States: ";
  for (size_t i = 0; i < states_.size() - 1; ++i) {
    if (states_[i]) {
      std::cout << i << "* ";
    } else {
      std::cout << i << " ";
    }
  }
  size_t i = states_.size() - 1;
  if (states_[i]) {
    std::cout << i << "*";
  } else {
    std::cout << i;
  }
  std::cout << "\n";
  std::cout << "Alphabet size: " << alphabet_.size() << "\n";
  std::cout << "Transitions:\n";
  for (auto& transition : transitions_) {
    std::cout << transition.curr_state << " "
              << alphabet_[transition.letter_ind] << " "
              << transition.next_state << "\n";
  }
}

void FSM::SortTransitions() {
  // 255_10 = 11111111_2
  std::vector<Transition> result = transitions_;
  size_t elem;
  for (size_t reversed = 0; reversed < 3; ++reversed) {
    elem = 2 - reversed;
    for (size_t i = 0; i < 8; ++i) {
      std::vector<size_t> count(256, 0);
      std::vector<size_t> indexes(256, 0);
      for (const auto& value : result) {
        ++count[(value[elem] >> (i * 8)) & 255];
      }
      for (size_t j = 0; j < 255; ++j) {
        indexes[j + 1] = indexes[j] + count[j];
      }
      for (const auto& value : transitions_) {
        result[indexes[(value[elem] >> (i * 8)) & 255]++] = value;
      }
      transitions_ = result;
    }
  }
}