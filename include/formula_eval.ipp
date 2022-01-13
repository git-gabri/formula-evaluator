#include "formula_eval.hpp"
#include <map>
#include <regex>
#include <iostream>
#include <cmath>

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
    const regex re_number("^[+-]?[0-9]+(?:[.]?[0-9]+)?(?:[eE][+-]?[0-9]+)?$");
    const map<string, base_math_operation> map_str_to_ops {
        {"+",       base_math_operation::add},
        {"-",       base_math_operation::sub},
        {"*",       base_math_operation::mul},
        {"/",       base_math_operation::div},
        {"root2",   base_math_operation::root2},
        {"sqrt",    base_math_operation::root2},
        {"root3",   base_math_operation::root3},
        {"cbrt",    base_math_operation::root3},
        {"root4",   base_math_operation::root4},
        {"root",    base_math_operation::root},
        {"exp",     base_math_operation::exp},
        {"exp2",    base_math_operation::exp2},
        {"exp10",   base_math_operation::exp10},
        {"pow",     base_math_operation::pow},
        {"ln",      base_math_operation::ln},
        {"log2",    base_math_operation::log2},
        {"log10",   base_math_operation::log10},
        {"log",     base_math_operation::log},
        {"sin",     base_math_operation::sin},
        {"cos",     base_math_operation::cos},
        {"tan",     base_math_operation::tan},
        {"asin",    base_math_operation::asin},
        {"acos",    base_math_operation::acos},
        {"atan",    base_math_operation::atan},
        {"sinh",    base_math_operation::sinh},
        {"cosh",    base_math_operation::cosh},
        {"tanh",    base_math_operation::tanh},
        {"asinh",   base_math_operation::asinh},
        {"acosh",   base_math_operation::acosh},
        {"atanh",   base_math_operation::atanh}
    };

    for(auto it = substrings.begin(); it != substrings.end(); ++it){
        if(map_str_to_ops.contains(*it)){
            ret_expr.ops.push_back(map_str_to_ops.at(*it));
        }
        else if(regex_match(*it, re_number)){
            ret_expr.ops.push_back(base_math_operation::num);
            ret_expr.constants.push_back(static_cast<T>(stold(*it)));
        }
        else{
            ret_expr.valid = false;
            break;
        }
    }

    //DEBUG
    /*
    if(ret_expr.valid == false)
        cout << "Invalid expression" << endl;
    const map<base_math_operation, string> map_ops_to_str {
        {base_math_operation::num, "NUMBER"},
        {base_math_operation::add, "+"},
        {base_math_operation::sub, "-"},
        {base_math_operation::mul, "*"},
        {base_math_operation::div, "/"},
        {base_math_operation::root2, "root2"},
        {base_math_operation::root3, "root3"},
        {base_math_operation::root4, "root4"},
        {base_math_operation::root, "root"},
        {base_math_operation::exp, "exp"},
        {base_math_operation::exp2, "exp2"},
        {base_math_operation::exp10, "exp10"},
        {base_math_operation::pow, "pow"},
        {base_math_operation::ln, "ln"},
        {base_math_operation::log2, "log2"},
        {base_math_operation::log10, "log10"},
        {base_math_operation::log, "log"},
        {base_math_operation::sin, "sin"},
        {base_math_operation::cos, "cos"},
        {base_math_operation::tan, "tan"},
        {base_math_operation::asin, "asin"},
        {base_math_operation::acos, "acos"},
        {base_math_operation::atan, "atan"},
        {base_math_operation::sinh, "sinh"},
        {base_math_operation::cosh, "cosh"},
        {base_math_operation::tanh, "tanh"},
        {base_math_operation::asinh, "asinh"},
        {base_math_operation::acosh, "acosh"},
        {base_math_operation::atanh, "atanh"}
    };
    for(auto it = ret_expr.ops.begin(); it != ret_expr.ops.end(); ++it){
        cout << map_ops_to_str.at(*it) << endl;
    }
    cout << "Numbers:" << endl;
    for(auto it = ret_expr.constants.begin(); it != ret_expr.constants.end(); ++it){
        cout << *it << endl;
    }
    */

    return ret_expr;
}

template<typename T>
evaluation_result_t<T> eval_expression(const expression_t<T>& expr){
    evaluation_result_t<T> result;

    //Check if expression is invalid, if it is, just exit
    if(!expr.valid){
        result.status = evaluation_status_t::invalid_expression;
        return result;
    }

    //Check if no operations have to be berformed
    if(expr.ops.empty()){
        return result;
    }

    //Stack into which all the calculations happen
    stack<T> calc_stack;
    //Map that contains the amount of arguments necessary for each operation
    const map<base_math_operation, size_t> map_num_args_per_op{
        {base_math_operation::num,      0},
        {base_math_operation::add,      2},
        {base_math_operation::sub,      2},
        {base_math_operation::mul,      2},
        {base_math_operation::div,      2},
        {base_math_operation::root2,    1},
        {base_math_operation::root3,    1},
        {base_math_operation::root4,    1},
        {base_math_operation::root,     2},
        {base_math_operation::exp,      1},
        {base_math_operation::exp2,     1},
        {base_math_operation::exp10,    1},
        {base_math_operation::pow,      2},
        {base_math_operation::ln,       1},
        {base_math_operation::log2,     1},
        {base_math_operation::log10,    1},
        {base_math_operation::log,      2},
        {base_math_operation::sin,      1},
        {base_math_operation::cos,      1},
        {base_math_operation::tan,      1},
        {base_math_operation::asin,     1},
        {base_math_operation::acos,     1},
        {base_math_operation::atan,     1},
        {base_math_operation::sinh,     1},
        {base_math_operation::cosh,     1},
        {base_math_operation::tanh,     1},
        {base_math_operation::asinh,    1},
        {base_math_operation::acosh,    1},
        {base_math_operation::atanh,    0}
    };
    //This is the index of the next constant to be loaded in the stack whenever the "num" op is found
    size_t constant_index = 0;

    //Iterating through all the operations to perform
    for(auto it = expr.ops.begin(); it != expr.ops.end(); ++it){
        //Checking if the stack contains enough elements to perform the current operation
        const size_t min_stack_size = map_num_args_per_op.at(*it);

        if(calc_stack.size() < min_stack_size){
            result.status = evaluation_status_t::missing_parameters;
            break;
        }

        //If the stack contains enough elements, perform the operations
        switch(*it){
            case base_math_operation::num:
                calc_stack.push(expr.constants[constant_index]);
                ++constant_index;
                break;

            case base_math_operation::add:
            {   const T b = calc_stack.top();
                calc_stack.pop();
                const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(a + b);
            }   break;

            case base_math_operation::sub:
            {   const T b = calc_stack.top();
                calc_stack.pop();
                const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(a - b);
            }   break;

            case base_math_operation::mul:
            {   const T b = calc_stack.top();
                calc_stack.pop();
                const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(a * b);
            }   break;

            case base_math_operation::div:
            {   const T b = calc_stack.top();
                calc_stack.pop();
                const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(a / b);
            }   break;

            case base_math_operation::root2:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(sqrt(a));
            }   break;

            case base_math_operation::root3:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(cbrt(a));
            }   break;

            case base_math_operation::root4:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(sqrt(sqrt(a)));
            }   break;

            case base_math_operation::root:
            {   const T a = calc_stack.top();
                calc_stack.pop();
                const T b = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(pow(a, 1/b));
            }   break;

            case base_math_operation::exp:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(exp(a));
            }   break;

            case base_math_operation::exp2:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(exp2(a));
            }   break;

            case base_math_operation::exp10:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(exp10(a));
                //calc_stack.push(pow(10, a));
            }   break;

            case base_math_operation::pow:
            {   const T b = calc_stack.top();
                calc_stack.pop();
                const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(pow(a, b));
            }   break;

            case base_math_operation::ln:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(log(a));
            }   break;

            case base_math_operation::log2:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(log2(a));
            }   break;

            case base_math_operation::log10:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(log10(a));
            }   break;

            case base_math_operation::log:
            {   const T a = calc_stack.top();
                calc_stack.pop();
                const T b = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(log2(a)/log2(b));
            }   break;

            case base_math_operation::sin:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(sin(a));
            }   break;

            case base_math_operation::cos:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(cos(a));
            }   break;

            case base_math_operation::tan:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(tan(a));
            }   break;

            case base_math_operation::asin:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(asin(a));
            }   break;

            case base_math_operation::acos:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(acos(a));
            }   break;

            case base_math_operation::atan:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(atan(a));
            }   break;

            case base_math_operation::sinh:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(sinh(a));
            }   break;

            case base_math_operation::cosh:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(cosh(a));
            }   break;

            case base_math_operation::tanh:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(tanh(a));
            }   break;

            case base_math_operation::asinh:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(asinh(a));
            }   break;

            case base_math_operation::acosh:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(acosh(a));
            }   break;

            case base_math_operation::atanh:
            {   const T a = calc_stack.top();
                calc_stack.pop();

                calc_stack.push(atanh(a));
            }   break;
        }
    }

    if(calc_stack.size() == 1)
        result.status = evaluation_status_t::good;
    else
        result.status = evaluation_status_t::stack_leftovers;

    result.value = calc_stack.top();

    return result;
}