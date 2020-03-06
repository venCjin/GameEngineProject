#include <Engine.h>
#include "Test.h"

// To run with Nvidia dedicated graphic accelerator
//extern "C"
//{
//	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

int main()
{
	Test t = Test();
	t.print();
	return 0;
}