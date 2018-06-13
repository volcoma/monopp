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

    test::test_suite("MONO TEST SUITE", [](auto& s)
    {
        monopp::test_suite(s);
        monort::test_suite(s);
        example::test_suite(s);
        
    });
    
	mono::shutdown();

	return 0;
}
