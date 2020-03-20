#include <Engine.h>
#include <EntryPoint.h>

class App : public sixengine::Application
{
public:
	App(std::string title, unsigned int width, unsigned int height)
		: Application(title, width, height)
	{
	}

	virtual void OnInit() override
	{
	}
};

sixengine::Application* sixengine::CreateApplication()
{
	return new App("App", 1280, 720);
}