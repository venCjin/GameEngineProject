#pragma once

#include <iostream>

namespace sixengine {

	class ISerializable
	{
		virtual void Load(std::iostream& stream) = 0;
		virtual void Save(std::iostream& stream) = 0;
	};

}
