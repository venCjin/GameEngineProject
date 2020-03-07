#include <Engine.h>

// To run with Nvidia dedicated graphic accelerator
//extern "C"
//{
//	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

int main()
{
	Test t = Test();
	t.work();
	return 0;
}