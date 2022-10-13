#include "DFSM.h"

void DFSM::Minimize() {
  std::vector<size_t> eq_classes(Size());
  size_t classes_count = DivideByFinal(eq_classes);
  classes_count = DivideClassses(eq_classes, classes_count);
  CreateMDFSM(eq_classes, classes_count);
}

void DFSM::CreateMDFSM(std::vector<size_t>& eq_classes, size_t classes_count) {
  std::unordered_map<size_t, size_t> new_state_ind;
  std::vector<State> new_states(classes_count);
  for (auto& state : new_states) {
    state.transitions.resize(GetAlphabetSize());
  }
  size_t curr_ind = 0;
  for (size_t i = 0; i < Size(); ++i) {
    if (eq_classes[i] == i) {
      new_states[curr_ind].final = IsFinal(i);
      new_state_ind[i] = curr_ind;
      for (size_t j = 0; j < GetAlphabetSize(); ++j) {
        new_states[curr_ind][j] = states_[i][j];
      }
      ++curr_ind;
    }
  }
  for (size_t state = 0; state < classes_count; ++state) {
    for (size_t letter = 0; letter < GetAlphabetSize(); ++letter) {
      size_t next_state = new_states[state][letter];
      next_state = new_state_ind[eq_classes[next_state]];
      new_states[state][letter] = next_state;
    }
  }
  states_ = std::move(new_states);
}

size_t DFSM::DivideClassses(std::vector<size_t>& eq_classes, size_t classes_count) {
  std::vector<size_t> new_eq_classes(Size());
  bool SizeMatch = false;
  while (!SizeMatch) {
    SizeMatch = true;
    for (size_t letter = 0; letter < GetAlphabetSize(); ++letter) {
      std::unordered_map<ClassId, size_t> new_class_ind;
      for (size_t state_from = 0; state_from < Size(); ++state_from) {
        ClassId id{};
        id.from = eq_classes[state_from];
        id.to = eq_classes[states_[state_from][letter]];
        if (!new_class_ind.contains(id)) {
          new_class_ind.insert({id, state_from});
          new_eq_classes[state_from] = state_from;
        } else {
          new_eq_classes[state_from] = new_class_ind[id];
        }
      }
      eq_classes = new_eq_classes;
      if (new_class_ind.size() != classes_count) {
        SizeMatch = false;
        classes_count = new_class_ind.size();
      }
    }
  }
  return classes_count;
}

size_t DFSM::DivideByFinal(std::vector<size_t>& classes) {
  size_t first_final = Size();
  size_t first_non_final = Size();
  size_t classes_count = 0;
  for (size_t i = 0; i < Size(); ++i) {
    if (IsFinal(i)) {
      if (first_final == Size()) {
        ++classes_count;
        first_final = i;
      }
      classes[i] = first_final;
    } else {
      if (first_non_final == Size()) {
        ++classes_count;
        first_non_final = i;
      }
      classes[i] = first_non_final;
    }
  }
  return classes_count;
}