#pragma once

namespace sixengine {

	void InitializeCore();
	void ShutdownCore();

}

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&##fn, this, std::placeholders::_1)