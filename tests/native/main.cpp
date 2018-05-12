#include "tests.h"

int main()
{
	if(false == mono::init("tests"))
		return 1;

	for(int i = 0; i < 20; ++i)
	{
		mono::mono_domain domain("test" + std::to_string(i));

		tests::test_mono(domain);
	}

	mono::shutdown();

	return 0;
}
