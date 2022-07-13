#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

/**
* The base class of almost every other game object. Made up of a single vertex array and texture, which are meant to be defined
by subclasses
*/
class Entity : public sf::Drawable, public sf::Transformable //Meant to be inherited
{
public:

	Entity();

	void setTexture(sf::Texture* tx);
	sf::Texture* getText() const;
	void setObj(sf::VertexArray oj); //Allows for more complex, user defined shapes
	sf::VertexArray getObj() const;

	virtual void handleEvents(sf::Event event);

protected:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::VertexArray obj;
	sf::Texture* text = nullptr;
};
