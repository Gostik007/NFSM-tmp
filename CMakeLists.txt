cmake_minimum_required(VERSION 3.23)
project(Finite_State_Machines)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Google_tests)
include_directories(Google_tests)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(Finite_State_Machines
        main.cpp
        src/FSM/FSM.cpp src/FSM/FSM.h
        Globals.cpp Globals.h
        src/NFSM.cpp src/NFSM.h
        src/DFSM/DFSM.cpp src/DFSM/DFSM.h
        src/DFSM/Minimization.cpp src/FSM/FSMTestMachineState.cpp)

target_include_directories(Finite_State_Machines PUBLIC ./Google_tests)