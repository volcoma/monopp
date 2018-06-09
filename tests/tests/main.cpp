#include "example_suite.h"
#include "monopp_suite.h"
#include "monort_suite.h"

#include "monopp/mono_jit.h"

int main()
{
	if(!mono::init("mono", true))
	{
		return 1;
	}

	test::describe("MONOPP TEST SUITE", monopp::test_suite);
	test::describe("MONORT TEST SUITE", example::test_suite);

	test::describe("EXAMPLE TEST SUITE", example::test_suite);

	mono::shutdown();

	return 0;
}
