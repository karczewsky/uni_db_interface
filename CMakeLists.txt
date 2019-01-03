cmake_minimum_required(VERSION 3.13)
project(Dziekanat)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpqxx -lpq")

find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

add_executable(Dziekanat main.cpp Person.cpp Person.h Student.cpp Student.h Utils.cpp Utils.h Persistable.h Db.cpp Db.h Group.cpp Group.h Teacher.cpp Teacher.h Module.cpp Module.h Grade.cpp Grade.h)
