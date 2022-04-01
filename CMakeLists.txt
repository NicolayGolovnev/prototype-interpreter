cmake_minimum_required(VERSION 3.15)
project(analyzator)

set(CMAKE_CXX_STANDARD 14)

add_executable(analyzator TestsScanner/TestScanner.cpp defs.h Scaner.h Scaner.cpp Diagram.cpp Diagram.h TestsDiagram/TestSyntax.cpp TestSemantic.cpp Semantic.h Semantic.cpp Triad.cpp Triad.h)