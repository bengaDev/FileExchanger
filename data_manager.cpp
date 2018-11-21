#include "data_manager.h"

Data_Manager::Data_Manager(std::string localHostName)
{
    bool visible = false;
    localHost = new Host(visible, localHostName);
}
