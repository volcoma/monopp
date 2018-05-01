#include "tests.h"

int main()
{
    if(false == mono::init("tests"))
        return 1;
    
    
    {     
        mono::mono_domain domain;
        tests::test_mono(domain); 
    }
    
    
    mono::shutdown();
    
    return 0;
}
