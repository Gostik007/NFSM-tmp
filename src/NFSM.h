#pragma once

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <bitset>
#include <unordered_map>
#include <unordered_set>
#include "../Globals.h"
#include "FSM/FSM.h"

struct NFSMCurrPosition {
  size_t state_ind;
  size_t pos_in_word;
  NFSMCurrPosition() = default;
  NFSMCurrPosition(size_t state_ind, size_t pos_in_word) : state_ind(state_ind),
  pos_in_word(pos_in_word) {}
};

namespace std {
template <>
struct hash<NFSMCurrPosition> {
  size_t operator()(const NFSMCurrPosition& id) const {
    return id.state_ind ^ (id.pos_in_word + 0x9e3779b9 + (id.state_ind << 6) + (id.state_ind >> 2));
  }
};
template<>
struct equal_to<NFSMCurrPosition> {
  bool operator()(const NFSMCurrPosition& lhs, const NFSMCurrPosition& rhs) const {
    return (lhs.state_ind == rhs.state_ind) && (lhs.pos_in_word == rhs.pos_in_word);
  }
};
}

class DFSM;

class NFSM {
  friend DFSM;
  struct State {
    bool final_;
    std::vector<size_t> epsilon_transitions_;
    // if bit 'i' in bitset is set to '1' than there is a transition to
    // the state 'i' by current letter
    // 'transitions' does not contain epsilon transitions
    std::vector<boost::dynamic_bitset<>> transitions_;
  };
  std::vector<State> states_;
  size_t bitset_size;

  void PrintTransition(size_t state_ind, size_t trans_ind);
  void HandleEpsilonTransitions(size_t curr_state_ind,
                                size_t prev_state_ind,
                                boost::dynamic_bitset<>& visited_states);
  bool IsAcceptedWithEpsilonTransitionsHelper(const std::vector<size_t>& word,
                                              std::unordered_set<NFSMCurrPosition>& visited_positions,
                                              size_t state_ind, size_t pos_in_word);

 public:
  explicit NFSM(FSM& fsm);
  explicit NFSM(const std::string& regex);

  void PullEpsilonTransitions();

  FSM GetFSM(const std::vector<SymbolType>& alphabet_,
             const std::unordered_map<SymbolType, size_t>& letter_to_num);

  void Print();

  bool IsAcceptedWithEpsilonTransitions(const std::vector<size_t>& word);

  bool Empty();
  size_t StatesCount() { return states_.size(); }
  size_t StatesCount() const { return states_.size(); }
};