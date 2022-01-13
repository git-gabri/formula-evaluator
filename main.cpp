#include <iostream>
#include "formula_eval.hpp"
#include "stopwatch.hpp"

using namespace std;

int main(int argc, char **argv){
	stopwatch s;
	cout << "Converting string to expression: ";

	s.tic();
	auto expr = convert_string_to_expression<double>(argv[1]);
	s.toc();
	cout << s << " seconds\n";

	if(!expr.valid){
		cout << "WARN: expression is invalid\n";
	}

	cout << "Evaluating expression: ";
	s.tic();
	auto res = eval_expression(expr);
	s.toc();
	cout << s << " seconds\n";

	cout << "Evaluation status: ";
	switch(res.status){
		case evaluation_status_t::good:
			cout << "good";
			break;

		case evaluation_status_t::stack_leftovers:
			cout << "good, but there were leftovers on the stack";
			break;

		case evaluation_status_t::invalid_expression:
			cout << "provided expression was invalid";
			break;

		case evaluation_status_t::missing_parameters:
			cout << "one or more parameters were missing during the evaluation";
			break;

		case evaluation_status_t::other:
			cout << "other";
			break;
	}
	cout << "\n";
	cout << "Final value: " << res.value << "\n";

	return 0;
}
