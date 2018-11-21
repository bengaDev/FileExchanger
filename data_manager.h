#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <list>
#include <host.h>

class Data_Manager
{
public:
    Data_Manager(std::string localHostName);


private:
    Host localHost;
    std::list<Host> online_users; // le liste devono avere un lock?
    //std::list<Host> to_send;
    //std::string file_path;
};

#endif // DATA_MANAGER_H
