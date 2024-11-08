#include <Nebula.h>

class TestGame : public Nebula::Application
{
public:
	TestGame()
	{
		PushOverlay(new Nebula::ImGuiLayer());
	}

	~TestGame()
	{
	}

};

Nebula::Application* Nebula::CreateApplication()
{
	return new TestGame;
}
