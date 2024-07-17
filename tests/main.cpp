#include "example_suite.h"
#include "monopp_suite.h"
#include "monort_suite.h"

#include <monopp/mono_jit.h>

int main()
{
	if(!mono::init({}, true))
	{
		return 1;
	}

	monopp::test_suite();
	monort::test_suite();
	//example::test_suite();

	mono::shutdown();

	return 0;
}
