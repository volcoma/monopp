#include "tests.h"
#include "monort/managed_interface/core/object.h"
int main()
{
	if(false == mono::init("tests"))
		return 1;
	{
		mono::mono_domain domain("test");
		
		
		tests::test_mono(domain);
	}


	mono::shutdown();

	return 0;
}
