#include "Controller.h"

void Controller::AddBind(sf::Keyboard::Key up, sf::Keyboard::Key down, float multiplier, std::function<void(float mult)> function)
{
	Binding bind;
	bind.Up = up;
	bind.Down = down;
	bind.Multiplier = multiplier;
	bind.Function = function;

	binds.push_back(bind);
}

void Controller::Update(float time)
{
	for (auto bind : binds)
	{
		if (sf::Keyboard::isKeyPressed(bind.Up) || sf::Keyboard::isKeyPressed(bind.Down))
		{
			float mult = sf::Keyboard::isKeyPressed(bind.Down) ? -time : time;
			bind.Function(mult * bind.Multiplier);
		}
	}
}