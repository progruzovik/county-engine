#ifndef EVENT_SPEAKER_HPP
#define EVENT_SPEAKER_HPP

#include <CE/Event/Listener.hpp>

namespace ce {

const sf::String UPDATE = "update";
const sf::String CLICK = "click";

class Speaker
{
public:
    Speaker(Listener *listener = nullptr);
    virtual ~Speaker() {}

    void setListener(Listener *value);

protected:
    void declareEvent(const sf::String &name);

private:
    Listener *listener;
};

}

#endif
