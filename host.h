#ifndef HOST_H
#define HOST_H
#include <string>

class Host
{
public:
    Host(bool visible = true, std::string name = "none");
    bool isVisible();

private:
    bool visible;
    std::string name;
    //address ip;
    //picture avatar;
};

#endif // HOST_H
