#ifndef F_EVAL_HPP_INCLUDED
#define F_EVAL_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <utility>

//The elements on the stack are referred as follow
//Bottom          Top
//  |              |
//  V              V
// btm [.....] z y x    <- Newly pushed elements end up here

//Base mathematical operations that can be performed on the elements of the stack
enum class base_operation{
    num,        //Push number on the stack
    dup,        //Duplicate x
    pop,        //Remove x
    swp         //Swap y and x
#if BASE_MATH_OPS
,   add,        //Add y + x
    sub,        //Subtract y - x
    mul,        //Multiply y * x
    div         //Divide y / x
#endif
#if ROOT_MATH_OPS
,   root2,      //Square root of x
    root3,      //Cubic root of x
    root4,      //4-th root of x
    root        //y-th root of x
#endif
#if EXP_MATH_OPS
,   exp,        //e^x
    exp2,       //2^x
    exp10,      //10^x
    pow         //y^x
#endif
#if LOG_MATH_OPS
,   ln,         //ln(x)
    log2,       //log2(x)
    log10,      //log10(x)
    log         //log in base y of x
#endif
#if TRIG_MATH_OPS
,   sin,        //sin(x)
    cos,        //cos(x)
    tan         //tan(x)
#endif
#if INVTRIG_MATH_OPS
,   asin,       //asin(x)
    acos,       //acos(x)
    atan        //atan(x)
#endif
#if HTRIG_MATH_OPS
,   sinh,       //sinh(x)
    cosh,       //cosh(x)
    tanh        //tanh(x)
#endif
#if INVHTRIG_MATH_OPS
,   asinh,      //asinh(x)
    acosh,      //acosh(x)
    atanh       //atanh(x)
#endif
};

//Associate a base_operation to the number of elements consumed and placed by it on the stack
//first:  number of elements consumed from the stack
//second: number of elements placed on the stack
const std::map<base_operation, std::pair<size_t, size_t>> map_op_to_args{
    {base_operation::num,      std::make_pair(0, 1)},
    {base_operation::dup,      std::make_pair(1, 2)},
    {base_operation::pop,      std::make_pair(1, 0)},
    {base_operation::swp,      std::make_pair(2, 2)}
#if BASE_MATH_OPS
,   {base_operation::add,      std::make_pair(2, 1)},
    {base_operation::sub,      std::make_pair(2, 1)},
    {base_operation::mul,      std::make_pair(2, 1)},
    {base_operation::div,      std::make_pair(2, 1)}
#endif
#if ROOT_MATH_OPS
,   {base_operation::root2,    std::make_pair(1, 1)},
    {base_operation::root3,    std::make_pair(1, 1)},
    {base_operation::root4,    std::make_pair(1, 1)},
    {base_operation::root,     std::make_pair(2, 1)}
#endif
#if EXP_MATH_OPS
,   {base_operation::exp,      std::make_pair(1, 1)},
    {base_operation::exp2,     std::make_pair(1, 1)},
    {base_operation::exp10,    std::make_pair(1, 1)},
    {base_operation::pow,      std::make_pair(2, 1)}
#endif
#if LOG_MATH_OPS
,   {base_operation::ln,       std::make_pair(1, 1)},
    {base_operation::log2,     std::make_pair(1, 1)},
    {base_operation::log10,    std::make_pair(1, 1)},
    {base_operation::log,      std::make_pair(2, 1)}
#endif
#if TRIG_MATH_OPS
,   {base_operation::sin,      std::make_pair(1, 1)},
    {base_operation::cos,      std::make_pair(1, 1)},
    {base_operation::tan,      std::make_pair(1, 1)}
#endif
#if INVTRIG_MATH_OPS
,   {base_operation::asin,     std::make_pair(1, 1)},
    {base_operation::acos,     std::make_pair(1, 1)},
    {base_operation::atan,     std::make_pair(1, 1)}
#endif
#if HTRIG_MATH_OPS
,   {base_operation::sinh,     std::make_pair(1, 1)},
    {base_operation::cosh,     std::make_pair(1, 1)},
    {base_operation::tanh,     std::make_pair(1, 1)}
#endif
#if INVHTRIG_MATH_OPS
,   {base_operation::asinh,    std::make_pair(1, 1)},
    {base_operation::acosh,    std::make_pair(1, 1)},
    {base_operation::atanh,    std::make_pair(0, 1)}
#endif
};

enum class evaluation_status_t {
    good,
    stack_leftovers,
    invalid_expression,
    missing_parameters,
    other
};

template<typename T>
struct expression_t{
    evaluation_status_t status{evaluation_status_t::good};
    std::vector<base_operation> ops;
    std::vector<T> constants;
};

template<typename T>
struct evaluation_result_t {
    T value{};
    evaluation_status_t status{evaluation_status_t::good};
};

template<typename T>
expression_t<T> convert_string_to_expression(const std::string& str);

//Function to simulate in advance how the stack will evolve during the computations specified by the given expression
template<typename T>
evaluation_status_t check_expression(const expression_t<T>& expr);

//Function to evaluate the expression.
//NOTE: during the computation, this doesn't check if the stack has enough elements or not to pop, so use the check_expression
//if you want to make sure this doesn't crash
template<typename T>
evaluation_result_t<T> eval_expression(const expression_t<T>& expr);

#include "formula_eval.ipp"

#endif
