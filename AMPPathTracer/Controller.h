#pragma once

#include <SFML/Window.hpp>

#include <vector>
#include <functional>

struct Binding
{
	sf::Keyboard::Key Up;
	sf::Keyboard::Key Down;
	std::function<void(float mult)> Function;
	float Multiplier;
};

class Controller
{
public:
	void AddBind(sf::Keyboard::Key up, sf::Keyboard::Key down, float multiplier, std::function<void(float mult)> function);

	void Update(float time);

private:
	std::vector<Binding> binds;
};