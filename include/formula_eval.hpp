#ifndef F_EVAL_HPP_INCLUDED
#define F_EVAL_HPP_INCLUDED

#include <string>
#include <vector>

template<typename T>
struct calculation_result{

};

enum class base_math_operation{
    num,
    add, sub, mul, div,
    root2, root3, root4, root,
    exp, exp2, exp10, pow,
    ln, log2, log10, log,
    sin, cos, tan,
    asin, acos, atan,
    sinh, cosh, tanh,
    asinh, acosh, atanh
};

template<typename T>
struct expression_t{
    bool valid = true;
    std::vector<base_math_operation> ops;
    std::vector<T> constants;
};

enum class evaluation_status_t {
    good,
    stack_leftovers,
    invalid_expression,
    missing_parameters,
    other
};

template<typename T>
struct evaluation_result_t {
    T value{};
    evaluation_status_t status{evaluation_status_t::good};
};

template<typename T>
expression_t<T> convert_string_to_expression(const std::string& str);

template<typename T>
evaluation_result_t<T> eval_expression(const expression_t<T>& expr);

#include "formula_eval.ipp"

#endif