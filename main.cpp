#include <iostream>
#include "formula_eval.hpp"
#include "stopwatch.hpp"

using namespace std;

int main(int argc, char **argv){
	if(argc < 2){
		cerr << "ERR: not enough arguments specified\n";
		return EXIT_FAILURE;
	}

	//Test string
	//const string expr_str("2 2 + s0 clr l0");
	stopwatch s;
	cout << "Converting string to expression: ";

	s.tic();
	auto expr = convert_string_to_expression<double>(argv[1]);
	//auto expr = convert_string_to_expression<double>(expr_str);
	s.toc();
	cout << s << " seconds" << endl;

	if(expr.status == evaluation_status_t::invalid_expression){
		cerr << "ERR: expression is invalid\n";
		return EXIT_FAILURE;
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

		case evaluation_status_t::empty:
			cout << "good, but stack was empty at the end of the computation";
			break;

		case evaluation_status_t::leftovers:
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

	return EXIT_SUCCESS;
}
