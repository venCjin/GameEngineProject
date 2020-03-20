#include <Engine.h>
#include <EntryPoint.h>

class Game : public sixengine::Application
{
public:
	Game(std::string title, unsigned int width, unsigned int height)
		: Application(title, width, height)
	{
	}

	virtual void OnInit() override
	{
	}
};

sixengine::Application* sixengine::CreateApplication()
{
	return new Game("App", 1280, 720);
}