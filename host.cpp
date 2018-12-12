#include "host.h"

Host::Host(bool visible, std::string name)
{
    this->visible = visible;
    this->name = name;
    avatar = new QPixmap(":/Icon_IMG/avatar_3.png");
}

bool Host::isVisible()
{
    return this->visible;
}

QPixmap Host::getAvatar(){
    return *avatar;
}
