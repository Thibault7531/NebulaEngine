
#include <Nebula.h>

class TestGame : public Nebula::Application
{
public:
	TestGame()
	= default;

	~TestGame()
	{

	}

};

Nebula::Application* Nebula::CreateApplication()
{
	return new TestGame;
}
