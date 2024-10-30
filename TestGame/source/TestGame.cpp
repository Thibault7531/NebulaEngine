#include <Nebula.h>

class ExampleLayer : public Nebula::Layer
{
public:
	ExampleLayer()
		: Layer("Exemple")
	{
	}

	void OnUpdate() override
	{
		NB_INFO("ExampleLayer::Update");
	}

	void OnEvent(Nebula::Event& event) override
	{
		NB_TRACE("{0}", event);
	}
};

class TestGame : public Nebula::Application
{
public:
	TestGame()
	{
		PushLayer(new ExampleLayer());
	}

	~TestGame()
	{
	}

};

Nebula::Application* Nebula::CreateApplication()
{
	return new TestGame;
}
