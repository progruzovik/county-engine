#include <CE/Core/AbstractNode.hpp>
#include <CE/constant.hpp>
#include <SFML/Window/Mouse.hpp>

namespace ce {

AbstractNode::AbstractNode(bool isSelectable, bool isUpdatable, ce::Listener *listener)
    : Speaker(listener), isSelectable(isSelectable), isUpdatable(isUpdatable) {}

AbstractNode::~AbstractNode()
{
    for (auto *child : children) {
        delete child;
    }
}

bool AbstractNode::checkSelectable() const
{
    return isSelectable;
}

void AbstractNode::setSelectable(bool value)
{
    if (isSelectable != value) {
        isSelectable = value;
        if (isSelectable && parent->checkChildSelected(this)) {
            onMouseEntered();
        } else {
            onMouseLeft();
        }
    }
}

const sf::Transform &AbstractNode::getCombinedTransform()
{
    if (isTransformed) {
        isTransformed = false;
        if (getParent()) {
            combinedTransform = getParent()->getCombinedTransform() * getTransformable().getTransform();
        } else {
            combinedTransform = getTransformable().getTransform();
        }
    }
    return combinedTransform;
}

AbstractNode *AbstractNode::getParent() const
{
    return parent;
}

const sf::Window &AbstractNode::getWindow() const
{
    return parent->getWindow();
}

float AbstractNode::getCenterX()
{
    return getWidth() / 2;
}

float AbstractNode::getCenterY()
{
    return getHeight() / 2;
}


float AbstractNode::getScale() const
{
    return getTransformable().getScale().x;
}

float AbstractNode::getRotation() const
{
    float rotation = getTransformable().getRotation() * ce::MATH_PI / 180;
    if (rotation > ce::MATH_PI) {
        rotation -= ce::MATH_PI * 2;
    }
    return rotation;
}

float AbstractNode::getOriginX() const
{
    return getTransformable().getOrigin().x;
}

void AbstractNode::setOriginX(float x)
{
    setOrigin(x, getOriginY());
}

float AbstractNode::getOriginY() const
{
    return getTransformable().getOrigin().y;
}

void AbstractNode::setOriginY(float y)
{
    setOrigin(getOriginX(), y);
}

float AbstractNode::getX() const
{
    return getTransformable().getPosition().x;
}

void AbstractNode::setX(float value)
{
    setPos(value, getY());
}

float AbstractNode::getY() const
{
    return getTransformable().getPosition().y;
}

void AbstractNode::setY(float value)
{
    setPos(getX(), value);
}

void AbstractNode::update()
{
    if (isUpdatable) {
        for (auto *child : children) {
            child->update();
        }
        onUpdated();
    }
}

void AbstractNode::select(const sf::Vector2i &mousePosition)
{
    deselectChild();
    if (checkSelectable()) {
        selectedChild = getSelectedChild(mousePosition);
        if (selectedChild) {
            selectedChild->onMouseEntered();
        }
    }
    if (selectedChild) {
        selectedChild->select(mousePosition);
    }
}

void AbstractNode::removeFromParent(bool toDelete)
{
    parent->removeChild(this, toDelete);
}

void AbstractNode::addChild(AbstractNode *child)
{
    child->setParent(this);
    children.push_back(child);
}

void AbstractNode::removeChild(AbstractNode *child, bool toDelete)
{
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        if (toDelete) {
            delete child;
        } else {
            child->setParent(nullptr);
        }
        children.erase(it);
    }
}

void AbstractNode::removeChildren(bool toDelete, unsigned long firstIndex, long lastIndex)
{
    const auto begin = children.begin() + firstIndex;
    const auto end = lastIndex == -1 || lastIndex > (long)children.size()
            ? children.end()
            : children.begin() + lastIndex;
    std::for_each(begin, end, [toDelete](AbstractNode *child) {
        if (toDelete) {
            delete child;
        } else {
            child->setParent(nullptr);
        }
    });
    children.erase(begin, end);
}

void AbstractNode::onUpdated()
{
    declareEvent(ce::UPDATED);
}

void AbstractNode::onMouseLeft()
{
    deselectChild();
}

void AbstractNode::onLeftMouseButtonPressed()
{
    if (selectedChild && selectedChild->checkSelectable()) {
        selectedChild->onLeftMouseButtonPressed();
    }
}

void AbstractNode::onLeftMouseButtonReleased()
{
    if (selectedChild && selectedChild->checkSelectable()) {
        selectedChild->onLeftMouseButtonReleased();
    }
}

void AbstractNode::onRightMouseButtonReleased()
{
    if (selectedChild && selectedChild->checkSelectable()) {
        selectedChild->onRightMouseButtonReleased();
    }
}

const std::vector<AbstractNode *> &AbstractNode::getChildren() const
{
    return children;
}

void AbstractNode::makeTransformed()
{
    isTransformed = true;
    for (auto *child : children) {
        child->makeTransformed();
    }
}

bool AbstractNode::checkMouseOnIt(const sf::Vector2i &mousePosition)
{
    const sf::Window &window = getWindow();
    sf::FloatRect combinedRect = getRect();
    if (parent) {
        combinedRect = parent->getCombinedTransform().transformRect(combinedRect);
    }
    return window.hasFocus() && mousePosition.x > 0 && mousePosition.x < (int) window.getSize().x
            && mousePosition.y > 0 && mousePosition.y < (int) window.getSize().y
            && mousePosition.x > combinedRect.left && mousePosition.x < combinedRect.left + combinedRect.width
            && mousePosition.y > combinedRect.top && mousePosition.y < combinedRect.top + combinedRect.height;
}

bool AbstractNode::checkChildSelected(const AbstractNode *child) const
{
    return selectedChild == child;
}

void AbstractNode::drawToTarget(sf::RenderTarget *target)
{
    for (auto *child : children) {
        child->drawToTarget(target);
    }
}

sf::Vector2f AbstractNode::calculateMouseLocalPosition()
{
    const sf::Vector2i mousePosition = sf::Mouse::getPosition(getWindow());
    const sf::FloatRect combinedRect = parent->getCombinedTransform().transformRect(getRect());
    const float combinedScale = combinedRect.width / getWidth();
    return sf::Vector2f((mousePosition.x - combinedRect.left) / combinedScale,
                        (mousePosition.y - combinedRect.top) / combinedScale);
}

void AbstractNode::setParent(AbstractNode *value)
{
    makeTransformed();
    parent = value;
}

AbstractNode *AbstractNode::getSelectedChild(const sf::Vector2i &mousePosition)
{
    auto it = std::find_if(children.rbegin(), children.rend(), [&mousePosition](AbstractNode *child) -> bool {
        return child->checkSelectable() && child->checkMouseOnIt(mousePosition);
    });
    return it == children.rend() ? nullptr : *it;
}

void AbstractNode::deselectChild()
{
    if (selectedChild) {
        selectedChild->onMouseLeft();
        selectedChild = nullptr;
    }
}

}
