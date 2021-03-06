cmake_minimum_required(VERSION 3.0.0)
project(formula_eval)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -pedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -pedantic -s -O2")

file (GLOB_RECURSE formula_eval_SOURCES CONFIGURE_DEPENDS "include/*.cpp")
file (GLOB_RECURSE formula_eval_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

set (formula_eval_INCLUDE_DIRS "")
foreach (_headerFile ${formula_eval_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND formula_eval_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES formula_eval_INCLUDE_DIRS)

add_executable(formula_eval main.cpp ${formula_eval_SOURCES})
target_include_directories(formula_eval PRIVATE ${formula_eval_INCLUDE_DIRS})

target_compile_definitions(formula_eval PUBLIC VARIABLES=true)
target_compile_definitions(formula_eval PUBLIC BASE_MATH_OPS=true)
target_compile_definitions(formula_eval PUBLIC ROOT_MATH_OPS=true)
target_compile_definitions(formula_eval PUBLIC EXP_MATH_OPS=true)
target_compile_definitions(formula_eval PUBLIC LOG_MATH_OPS=true)
target_compile_definitions(formula_eval PUBLIC TRIG_MATH_OPS=true)
target_compile_definitions(formula_eval PUBLIC INVTRIG_MATH_OPS=true)
target_compile_definitions(formula_eval PUBLIC HTRIG_MATH_OPS=true)
target_compile_definitions(formula_eval PUBLIC INVHTRIG_MATH_OPS=true)