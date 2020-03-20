#include "pch.h"

namespace sixengine
{
	class Example
	{
	private:
		int m_field;		// member field
		static float s_var; // static field

	public:
		Example(int field, float var) : m_field(field), s_var(var)
		{
			Init();
		}

		float Fun()
		{
			return s_var * m_field;
		}

		void Increment()
		{
			s_var++;
		}

		void SetField(int field)
		{
			m_field = field;
		}

		void SetVar(float var)
		{
			s_var = var;
		}

		int GetField()
		{
			return m_field;
		}

		float GetVar()
		{
			return s_var;
		}

	private:
		void Init()
		{
			// code ...
		}
	};
} // namespace SixEngine

int main()
{
	Example ex(1, 1.667f);
	float ex_i = ex.Fun();
	if (ex_i > 2.0f)
	{
		// code ...
	}
	else
	{
		// code ...
	}
}