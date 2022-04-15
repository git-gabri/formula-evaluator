#include "formula_eval.hpp"
#include <map>
#include <regex>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

template<typename T>
expression_t<T> convert_string_to_expression(const string& s){
    expression_t<T> ret_expr;

    //Separate the individual "words" (sequence of characteres delimited by spaces) and put them in the substrings vector
    vector<string> substrings;
    {   
        auto it = s.begin();
        string tmp;
        const string delimiters = " ";

        while(it != s.end()){
            if(delimiters.find(*it) == string::npos) {
                tmp += *it;
            }
            else {
                substrings.push_back(tmp);
                tmp.clear();
            }

            ++it;
        }

        if(!tmp.empty())
            substrings.push_back(tmp);
    }

    //Convert the elements in substrings into base mathematical operations
    //Auxiliary variables
    const regex re_number(R"foo(^[+-]?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?$)foo");
#if VARIABLES
    const regex re_var_store(R"foo(^s\d+$)foo");
    const regex re_var_load (R"foo(^l\d+$)foo");
#endif
    const map<string, base_operation> map_str_to_ops {
        {"dup",     base_operation::dup},
        {"pop",     base_operation::pop},
        {"clr",     base_operation::clr},
        {"swp",     base_operation::swp}
#if BASE_MATH_OPS
    ,   {"+",       base_operation::add},
        {"-",       base_operation::sub},
        {"*",       base_operation::mul},
        {"/",       base_operation::div}
#endif
#if ROOT_MATH_OPS
    ,   {"root2",   base_operation::root2},
        {"sqrt",    base_operation::root2},
        {"root3",   base_operation::root3},
        {"cbrt",    base_operation::root3},
        {"root4",   base_operation::root4},
        {"root",    base_operation::root}
#endif
#if EXP_MATH_OPS
    ,   {"exp",     base_operation::exp},
        {"exp2",    base_operation::exp2},
        {"exp10",   base_operation::exp10},
        {"pow",     base_operation::pow}
#endif
#if LOG_MATH_OPS
    ,   {"ln",      base_operation::ln},
        {"log2",    base_operation::log2},
        {"log10",   base_operation::log10},
        {"log",     base_operation::log}
#endif
#if TRIG_MATH_OPS
    ,   {"sin",     base_operation::sin},
        {"cos",     base_operation::cos},
        {"tan",     base_operation::tan}
#endif
#if INVTRIG_MATH_OPS
    ,   {"asin",    base_operation::asin},
        {"acos",    base_operation::acos},
        {"atan",    base_operation::atan}
#endif
#if HTRIG_MATH_OPS
    ,   {"sinh",    base_operation::sinh},
        {"cosh",    base_operation::cosh},
        {"tanh",    base_operation::tanh}
#endif
#if INVHTRIG_MATH_OPS
    ,   {"asinh",   base_operation::asinh},
        {"acosh",   base_operation::acosh},
        {"atanh",   base_operation::atanh}
#endif
    };

    for(auto it = substrings.begin(); it != substrings.end(); ++it){
        if(map_str_to_ops.contains(*it)){
            ret_expr.ops.push_back(map_str_to_ops.at(*it));
        }
        else if(regex_match(*it, re_number)){
            ret_expr.ops.push_back(base_operation::num);
            ret_expr.constants.push_back(static_cast<T>(stold(*it)));
        }
#if VARIABLES
        else if(regex_match(*it, re_var_store)){
            ret_expr.ops.push_back(base_operation::st);
            ret_expr.accessed_vars_indexes.push_back(static_cast<T>(stoull((*it).substr(1))));
        }
        else if(regex_match(*it, re_var_load)){
            ret_expr.ops.push_back(base_operation::ld);
            ret_expr.accessed_vars_indexes.push_back(static_cast<T>(stoull((*it).substr(1))));
        }
#endif
        else{
            ret_expr.status = evaluation_status_t::invalid_expression;
            break;
        }
    }

    if(ret_expr.status != evaluation_status_t::invalid_expression)
        ret_expr.status = check_expression(ret_expr);

    return ret_expr;
}

template<typename T>
evaluation_status_t check_expression(const expression_t<T>& expr){
    size_t stack_size = 0;

    for(auto it = expr.ops.begin(); it != expr.ops.end(); ++it){
        if(*it == base_operation::clr){
            stack_size = 0;
            continue;
        }

        if(stack_size < map_op_to_args.at(*it).first)
            return evaluation_status_t::missing_parameters;

        stack_size -= map_op_to_args.at(*it).first;
        stack_size += map_op_to_args.at(*it).second;
    }

    if(stack_size == 0)
        return evaluation_status_t::empty;
    else if(stack_size == 1)
        return evaluation_status_t::good;
    else
        return evaluation_status_t::leftovers;
}

template<typename T>
evaluation_result_t<T> eval_expression(const expression_t<T>& expr){
    evaluation_result_t<T> result;

    //Check if expression is invalid or has missing parameters. If so, exit the function
    if(expr.status == evaluation_status_t::invalid_expression || expr.status == evaluation_status_t::missing_parameters){
        result.status = expr.status;
        return result;
    }

    //Check if no operations have to be berformed
    if(expr.ops.empty()){
        return result;
    }

    //Stack into which all the calculations happen
    stack<T> calc_stack;
    //This is the index of the next constant to be loaded in the stack whenever the "num" op is found
    size_t constant_index = 0;

#if VARIABLES
    //Map into which all variables are stored
    map<size_t, T> var_map{};
    //This is the index of the next variable to be accessed, either in store or load, whenever the "sXYZ / lXYZ" ops are found
    size_t var_index = 0;
#endif

    //Iterating through all the operations to perform
    for(auto it = expr.ops.begin(); it != expr.ops.end(); ++it){
        //Perform the operations
        switch(*it){
            case base_operation::num:
                calc_stack.push(expr.constants[constant_index]);
                ++constant_index;
                break;

            case base_operation::dup:
                calc_stack.push(calc_stack.top());
                break;

            case base_operation::pop:
                calc_stack.pop();
                break;

            case base_operation::clr:
                calc_stack = stack<T>{};
                break;

            case base_operation::swp:
            {   const T x = calc_stack.top();
                calc_stack.pop();
                const T y = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(x);
                calc_stack.push(y);
            }   break;
#if VARIABLES
            case base_operation::st:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                var_map[expr.accessed_vars_indexes[var_index]] = x;
                ++var_index;
            }   break;

            case base_operation::ld:
                calc_stack.push(var_map[expr.accessed_vars_indexes[var_index]]);
                ++var_index;
                break;
#endif
#if BASE_MATH_OPS
            case base_operation::add:
            {   const T x = calc_stack.top();
                calc_stack.pop();
                const T y = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(y + x);
            }   break;

            case base_operation::sub:
            {   const T x = calc_stack.top();
                calc_stack.pop();
                const T y = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(y - x);
            }   break;

            case base_operation::mul:
            {   const T x = calc_stack.top();
                calc_stack.pop();
                const T y = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(y * x);
            }   break;

            case base_operation::div:
            {   const T x = calc_stack.top();
                calc_stack.pop();
                const T y = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(y / x);
            }   break;
#endif
#if ROOT_MATH_OPS
            case base_operation::root2:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(sqrt(x));
            }   break;

            case base_operation::root3:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(cbrt(x));
            }   break;

            case base_operation::root4:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(sqrt(sqrt(x)));
            }   break;

            case base_operation::root:
            {   const T x = calc_stack.top();
                calc_stack.pop();
                const T y = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(pow(x, 1/y));
            }   break;
#endif
#if EXP_MATH_OPS
            case base_operation::exp:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(exp(x));
            }   break;

            case base_operation::exp2:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(exp2(x));
            }   break;

            case base_operation::exp10:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(exp10(x));
                //calc_stack.push(pow(10, a));
            }   break;

            case base_operation::pow:
            {   const T x = calc_stack.top();
                calc_stack.pop();
                const T y = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(pow(y, x));
            }   break;
#endif
#if LOG_MATH_OPS
            case base_operation::ln:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(log(x));
            }   break;

            case base_operation::log2:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(log2(x));
            }   break;

            case base_operation::log10:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(log10(x));
            }   break;

            case base_operation::log:
            {   const T x = calc_stack.top();
                calc_stack.pop();
                const T y = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(log2(x)/log2(y));
            }   break;
#endif
#if TRIG_MATH_OPS
            case base_operation::sin:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(sin(x));
            }   break;

            case base_operation::cos:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(cos(x));
            }   break;

            case base_operation::tan:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(tan(x));
            }   break;
#endif
#if INVTRIG_MATH_OPS
            case base_operation::asin:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(asin(x));
            }   break;

            case base_operation::acos:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(acos(x));
            }   break;

            case base_operation::atan:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(atan(x));
            }   break;
#endif
#if HTRIG_MATH_OPS
            case base_operation::sinh:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(sinh(x));
            }   break;

            case base_operation::cosh:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(cosh(x));
            }   break;

            case base_operation::tanh:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(tanh(x));
            }   break;
#endif
#if INVHTRIG_MATH_OPS
            case base_operation::asinh:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(asinh(x));
            }   break;

            case base_operation::acosh:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(acosh(x));
            }   break;

            case base_operation::atanh:
            {   const T x = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(atanh(x));
            }   break;
#endif
        }
    }

    result.status = expr.status;
    if(expr.status == evaluation_status_t::empty)
        result.value = 0;
    else
        result.value = calc_stack.top();

    return result;
}