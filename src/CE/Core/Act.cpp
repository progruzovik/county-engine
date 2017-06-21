#include <CE/Core/Act.hpp>
#include <CE/UI/Parameters.hpp>
#include <SFML/Window/Mouse.hpp>

namespace ce {

Act::Act(Stage *stage, Mode contentMode, const sf::Color &bgColor)
    : Speaker(stage), bgColor(bgColor), contentMode(contentMode),
      savedMousePosition(sf::Mouse::getPosition(*stage)), window(*stage)
{
    addChild(contentLayer);
}

void Act::onMouseMoved(const sf::Vector2i &mousePosition)
{
    Node *newSelectedNode = select(mousePosition);
    if (selectedNode != newSelectedNode) {
        if (selectedNode) {
            selectedNode->onMouseLeft();
        }
        selectedNode = newSelectedNode;
        if (selectedNode) {
            selectedNode->onMouseEntered();
        }
    }
    if (selectedNode) {
        selectedNode->onMouseMoved(mousePosition);
    }

    if (contentMode == Mode::MOVABLE_BY_MOUSE) {
        if (isRightMouseButtonPressed) {
            sf::Vector2i offset(mousePosition.x - savedMousePosition.x, mousePosition.y - savedMousePosition.y);
            if (std::abs(offset.x) + std::abs(offset.y) > 10) {
                isMouseMovedWithRightButton = true;
            }
            content->move(offset.x, offset.y);
        }
        savedMousePosition = mousePosition;
    }
}

void Act::onMouseLeft()
{
    if (selectedNode) {
        selectedNode->onMouseLeft();
        selectedNode = nullptr;
    }
}

void Act::onLeftMouseButtonPressed()
{
    if (selectedNode) {
        if (selectedNode->checkSelectable()) {
            selectedNode->onLeftMouseButtonPressed();
        } else {
            selectedNode = nullptr;
        }
    }
}

void Act::onLeftMouseButtonReleased()
{
    if (selectedNode) {
        if (selectedNode->checkSelectable()) {
            selectedNode->onLeftMouseButtonReleased();
        } else {
            selectedNode = nullptr;
        }
    }
}

void Act::onRightMouseButtonPressed()
{
    if (contentMode == Mode::MOVABLE_BY_MOUSE) {
        isRightMouseButtonPressed = true;
        isMouseMovedWithRightButton = false;
    }
}

void Act::onRightMouseButtonReleased()
{
    if (contentMode == Mode::MOVABLE_BY_MOUSE) {
        isRightMouseButtonPressed = false;
    }

    if (!isMouseMovedWithRightButton && selectedNode) {
        if (selectedNode->checkSelectable()) {
            selectedNode->onRightMouseButtonReleased();
        } else {
            selectedNode = nullptr;
        }
    }
}

const sf::Color &Act::getBgColor() const
{
    return bgColor;
}

const sf::Transform &Act::getCombinedTransform()
{
    return mockTransform;
}

const sf::Window &Act::getWindow() const
{
    return window;
}

void Act::setCenter(TransformableNode *value)
{
    center = value;
}

void Act::setContent(TransformableNode *value)
{
    if (content) {
        contentLayer->removeChild(content);
    }
    content = value;
    contentLayer->addChild(content);
    setUpNodes();
}

void Act::setLeftUi(TransformableNode *value)
{
    updateUi(leftUi, value);
    leftUi = value;
}

void Act::setRightUi(TransformableNode *value)
{
    updateUi(rightUi, value);
    rightUi = value;
}

void Act::setTopUi(TransformableNode *value)
{
    updateUi(topUi, value);
    topUi = value;
}

void Act::setBottomUi(TransformableNode *value)
{
    updateUi(bottomUi, value);
    bottomUi = value;
}

void Act::setUpNodes()
{
    resizeUi();
    const float fullLeftIndent = leftUi ? leftUi->getWidth() : 0;
    const float fullTopIndent = topUi ? topUi->getHeight() : 0;

    if (leftUi) {
        leftUi->setY(fullTopIndent);
    }
    float freeWidth = window.getSize().x - fullLeftIndent;
    if (rightUi) {
        rightUi->setPos(window.getSize().x - rightUi->getWidth(), fullTopIndent); //rightUi->getWidth()!!! 330 vs 165
        freeWidth -= rightUi->getWidth();
    }

    if (topUi) {
        topUi->setX(fullLeftIndent);
    }
    float freeHeight = window.getSize().y - fullTopIndent;
    if (bottomUi) {
        bottomUi->setPos(fullLeftIndent, window.getSize().y - bottomUi->getHeight());
        freeHeight -= bottomUi->getHeight();
    }

    if (contentMode == Mode::STATIC) {
        contentLayer->setScale(std::min(freeWidth / contentLayer->getWidth(), freeHeight / contentLayer->getHeight()));
        contentLayer->setOrigin(contentLayer->getHalfX(), contentLayer->getHalfY());
        contentLayer->setPos(fullLeftIndent + freeWidth / 2, fullTopIndent + freeHeight / 2);
    } else {
        contentLayer->setScale(Parameters::get().getK());
    }
}

void Act::update()
{
    Node::update();

    if (contentMode == Mode::MOVABLE_BY_MOUSE) {
        sf::Vector2i offset;
        const unsigned int activeArea = Parameters::get().getIndent() / 2;
        if (savedMousePosition.x < activeArea) {
            offset.x += SCROLL_SPEED * Parameters::get().getK();
        } else if (savedMousePosition.x > window.getSize().x - activeArea) {
            offset.x -= SCROLL_SPEED * Parameters::get().getK();
        }
        if (savedMousePosition.y < activeArea) {
            offset.y += SCROLL_SPEED * Parameters::get().getK();
        } else if (savedMousePosition.y > window.getSize().y - activeArea) {
            offset.y -= SCROLL_SPEED * Parameters::get().getK();
        }
        contentLayer->move(offset.x, offset.y);
    } else if (contentMode == Mode::CENTERED_ON_NODE) {
        sf::Vector2f offset = center->getCombinedTransform().transformPoint(0, 0);
        contentLayer->move(window.getSize().x / 2 - offset.x, window.getSize().y / 2 - offset.y);
    }

    drawToTarget(window);
}

bool Act::checkPointOnIt(const sf::Vector2i &point)
{
    return point.x > 0 && point.x < window.getSize().x && point.y > 0 && point.y < window.getSize().y;
}

void Act::updateUi(TransformableNode *oldUi, TransformableNode *newUi)
{
    if (oldUi) {
        removeChild(oldUi);
    }
    if (newUi) {
        addChild(newUi);
    }
}

}
