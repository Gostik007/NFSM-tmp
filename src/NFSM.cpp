#include "NFSM.h"

void NFSM::PrintTransition(size_t state_ind, size_t trans_ind) {
  for (boost::dynamic_bitset<>::size_type i = 0;
       i < states_[state_ind].transitions_[trans_ind].size(); ++i) {
    if (states_[state_ind].transitions_[trans_ind][i]) {
      std::cout << i << " ";
    }
  }
}

void NFSM::HandleEpsilonTransitions(size_t curr_state_ind,
                                    size_t prev_state_ind,
                                    boost::dynamic_bitset<>& visited_states) {
  if (visited_states.test(curr_state_ind)) {
    return;
  }
  // handling transitions
  if (!states_[curr_state_ind].epsilon_transitions_.empty()) {
    visited_states.set(curr_state_ind, true);
    for (auto& epsilon_transition_index : states_[curr_state_ind].epsilon_transitions_) {
      HandleEpsilonTransitions(epsilon_transition_index, curr_state_ind, visited_states);
    }
    visited_states.set(curr_state_ind, false);
  }
  // handling final states
  if (curr_state_ind != prev_state_ind && states_[curr_state_ind].final_) {
    states_[prev_state_ind].final_ = true;
  }
  if (curr_state_ind != prev_state_ind) {
    for (size_t i = 0; i < states_[prev_state_ind].transitions_.size(); ++i) {
      states_[prev_state_ind].transitions_[i] |= states_[curr_state_ind].transitions_[i];
    }
  }
}

NFSM::NFSM(FSM& fsm) {
  states_.resize(fsm.states_.size());
  for (size_t i = 0; i < states_.size(); ++i) {
    states_[i].final_ = fsm.states_[i];
    bitset_size = fsm.alphabet_.size() - 1;
    states_[i].transitions_.resize(bitset_size,
                                   boost::dynamic_bitset<>(states_.size(), 0));
  }
  for (auto& transition : fsm.transitions_) {
    if (transition.letter_ind == 0) {
      states_[transition.curr_state].epsilon_transitions_
          .push_back(transition.next_state);
    } else {
      states_[transition.curr_state].transitions_[transition.letter_ind - 1]
          .set(transition.next_state, true);
    }
  }
}

NFSM::NFSM(const std::string& regex) {

}

void NFSM::PullEpsilonTransitions() {
  std::vector<size_t> stack_indexes;
  stack_indexes.reserve(states_.size());
  boost::dynamic_bitset<> visited_states(states_.size(), 0);
  for (size_t i = 0; i < states_.size(); ++i) {
    HandleEpsilonTransitions(i, i, visited_states);
  }
  for (auto& state : states_) {
    state.epsilon_transitions_.clear();
  }
}

void NFSM::Print() {
  std::cout << "--------NFSM--------\n";
  for (size_t i = 0; i < states_.size(); ++i) {
    if (states_[i].final_) {
      std::cout << "STATE " << i << "*:\n";
    } else {
      std::cout << "STATE " << i << ":\n";
    }
    if (!states_[i].epsilon_transitions_.empty()) {
      std::cout << "EPSILON: ";
      for (auto& transition : states_[i].epsilon_transitions_) {
        std::cout << transition << " ";
      }
      std::cout << "\n";
    }
    for (size_t j = 0; j < states_[i].transitions_.size(); ++j) {
      std::cout << "transition by " << j << " to ";
      PrintTransition(i, j);
      std::cout << "\n";
    }
    std::cout << "\n";
  }
}

bool NFSM::Empty() {
  return states_.empty();
}

FSM NFSM::GetFSM(const std::vector<SymbolType>& alphabet_,
                 const std::unordered_map<SymbolType, size_t>& letter_to_num) {
  FSM fsm;
  fsm.states_.resize(states_.size());
  fsm.alphabet_ = alphabet_;
  fsm.letter_to_num_ = letter_to_num;
  for (size_t i = 0; i < states_.size(); ++i) {
    fsm.states_[i] = states_[i].final_;
    for (auto eTransition : states_[i].epsilon_transitions_) {
      fsm.transitions_.emplace_back(i, 0, eTransition);
    }
    for (size_t letter_ind = 0; letter_ind < states_[i].transitions_.size(); ++letter_ind) {
      for (boost::dynamic_bitset<>::size_type next_state = 0;
           next_state < states_[i].transitions_[letter_ind].size(); ++next_state) {
        if (states_[i].transitions_[letter_ind][next_state]) {
          fsm.transitions_.emplace_back(i, letter_ind + 1, next_state);
        }
      }
    }
  }
  return fsm;
}

void FSM::PullEpsilonTransitions() {
  class NFSM nfsm(*this);
  nfsm.PullEpsilonTransitions();
  *this = nfsm.GetFSM(alphabet_, letter_to_num_);
}

bool NFSM::IsAcceptedWithEpsilonTransitionsHelper(const std::vector<size_t>& word,
                                                  std::unordered_set<NFSMCurrPosition>& visited_positions,
                                                  size_t state_ind, size_t pos_in_word) {
  if (visited_positions.contains({state_ind, pos_in_word})) {
    return false;
  } else {
    visited_positions.emplace(state_ind, pos_in_word);
  }
  if (pos_in_word == word.size()) {
    return states_[state_ind].final_;
  }
  for (auto&& next_state : states_[state_ind].epsilon_transitions_) {
    if (IsAcceptedWithEpsilonTransitionsHelper(word,
                                               visited_positions,
                                               next_state,
                                               pos_in_word)) {
      return true;
    }
  }
  size_t curr_symbol_ind = word[pos_in_word] - 1;
  for (size_t i = 0; i < states_[state_ind].transitions_[curr_symbol_ind].size(); ++i) {
    if (states_[state_ind].transitions_[word[pos_in_word]][i]) {
      if (IsAcceptedWithEpsilonTransitionsHelper(word,
                                                 visited_positions,
                                                 i,
                                                 pos_in_word + 1)) {
        return true;
      }
    }
  }
  return false;
}

bool NFSM::IsAcceptedWithEpsilonTransitions(const std::vector<size_t>& word) {
  if (Empty()) {
    return false;
  }
  if (word.empty() || word[0] == 0) {
    return states_[0].final_;
  }
  std::unordered_set<NFSMCurrPosition> visited_positions;
  for (size_t epsilon_transition : states_[0].epsilon_transitions_) {
    if (IsAcceptedWithEpsilonTransitionsHelper(word,
                                               visited_positions,
                                               epsilon_transition,
                                               0)) {
      return true;
    }
  }
  size_t curr_symbol_ind = word[0] - 1;
  for (size_t i = 0; i < states_[0].transitions_[curr_symbol_ind].size(); ++i) {
    if (states_[0].transitions_[word[0]][i]) {
      if (IsAcceptedWithEpsilonTransitionsHelper(word, visited_positions, i, 1)) {
        return true;
      }
    }
  }
  return false;
}

bool FSM::NFSMIsAccepted(const std::vector<size_t>& word) {
  class NFSM nfsm(*this);
  return nfsm.IsAcceptedWithEpsilonTransitions(word);
}