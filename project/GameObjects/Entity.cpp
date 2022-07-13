#include <iostream>
#include <SFML/Graphics.hpp>
#include "Entity.h"

Entity::Entity() {}

void Entity::setTexture(sf::Texture* tx) { text = tx; }
void Entity::setObj(sf::VertexArray oj) { obj = oj; }

void Entity::handleEvents(sf::Event event) {}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	states.transform.combine(getTransform());
	states.texture = text;
	target.draw(obj, states);
}

sf::VertexArray Entity::getObj() const { return obj; }
sf::Texture* Entity::getText() const { return text; }