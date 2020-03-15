#include "pch.h"

class Example {
private:
	int m_field;          // member field
	static float s_var;   // static field

public:
	Example(int field, float var) : m_field(field), s_var(var)
	{
		init();
	}

	float Fun()
	{
		return s_var * m_field;
	}

private:
	void init()
	{
		// code ...
	}
};

int main()
{
	Example ex(1, 1.667f);
	float ex_i = ex.Fun();
	if (ex_i > 1.0f)
	{
		// code ...
	}
	else
	{
		// code ...
	}
}