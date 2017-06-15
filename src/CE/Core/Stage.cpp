#include <CE/Core/Stage.hpp>
#include <CE/Core/AbstractAct.hpp>
#include <CE/UI/Parameters.hpp>
#include <SFML/Window/Event.hpp>

namespace ce {

Stage::Stage(const sf::VideoMode &mode, const sf::String &title, AbstractAct *currentAct)
    : sf::RenderWindow(mode, title), currentAct(currentAct), view(sf::FloatRect(0, 0, mode.width, mode.height))
{
    setVerticalSyncEnabled(true);
    updateView();
}

Stage::~Stage()
{
    if (currentAct) {
        delete currentAct;
    }
}

sf::View &Stage::getMutableView()
{
    return view;
}

void Stage::start()
{
    sf::Clock clock;
    while (isOpen()) {
        update();
    }
}

void Stage::update()
{
    sf::Event event;
    while (pollEvent(event)) {
        if (event.type == sf::Event::Resized) {
            ce::Parameters::get().update(event.size.width, event.size.height);
            view.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));
            updateView();
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                currentAct->onLeftMouseButtonPressed();
            } else if (event.mouseButton.button == sf::Mouse::Right) {
                currentAct->onRightMouseButtonPressed();
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                currentAct->onLeftMouseButtonReleased();
            } else if (event.mouseButton.button == sf::Mouse::Right) {
                currentAct->onRightMouseButtonReleased();
            }
        } else if (event.type == sf::Event::Closed) {
            close();
        }
    }

    clear(currentAct->getBgColor());
    currentAct->update();
    display();
}

void Stage::updateView()
{
    currentAct->setUpNodes();
    setView(view);
}

}
