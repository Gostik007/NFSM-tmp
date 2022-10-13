#pragma once

#include <queue>
#include <utility>
#include "../NFSM.h"

struct ClassId {
  size_t from;
  size_t to;
};

namespace std {
template <>
struct hash<ClassId> {
  size_t operator()(const ClassId& id) const {
    return id.from ^ (id.to + 0x9e3779b9 + (id.from << 6) + (id.from >> 2));
  }
};
template<>
struct equal_to<ClassId> {
  bool operator()(const ClassId& lhs, const ClassId& rhs) const {
    return (lhs.from == rhs.from) && (lhs.to == rhs.to);
  }
};
}

class DFSM {
  struct StateBitset {
    bool final;
    std::vector<boost::dynamic_bitset<>> transitions;
  };

  struct State {
    bool final;
    // letter - index
    // value - result state
    std::vector<size_t> transitions; // -1 if no transition (-1 == MAX_SIZE_T)
    size_t& operator[](size_t i) {
      return transitions[i];
    }
  };
  std::vector<State> states_;

 public:
  DFSM() = default;
  explicit DFSM(NFSM& nfsm);

  void Print();

  void CreateFrom(NFSM& nfsm);

  FSM GetFSM(const std::vector<SymbolType>& alphabet_,
             const std::unordered_map<SymbolType, size_t>& letter_to_num);

  //  Minimization
  void Minimize();
  void CreateMDFSM(std::vector<size_t>& eq_classes, size_t classes_count);
  size_t DivideClassses(std::vector<size_t>& eq_classes, size_t classes_count);
  size_t DivideByFinal(std::vector<size_t>& classes);

  size_t GetAlphabetSize();
  size_t Size() { return states_.size(); }
  State& operator[](size_t i) { return states_[i]; }
  bool IsFinal(size_t i) { return states_[i].final; }
  bool Empty() { return states_.empty(); }
};