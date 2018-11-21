#include "host.h"

Host::Host(bool visible, std::string name)
{
    this->visible = visible;
    this->name = name;
}

bool Host::isVisible()
{
    return this->visible;
}
