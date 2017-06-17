#ifndef CE_STAGE_HPP
#define CE_STAGE_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <CE/Event/Listener.hpp>

namespace ce {

class Act;

class Stage : public sf::RenderWindow, public Listener
{
public:
    Stage(const sf::VideoMode &mode, const sf::String &title, Act *currentAct);
    ~Stage() override;

    void onEvent(Speaker *speaker, const sf::String &name) override {}
    void start();

protected:
    Act *currentAct;

    virtual void update();

private:
    sf::View view;

    void updateView();
};

}

#endif
