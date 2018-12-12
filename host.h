#ifndef HOST_H
#define HOST_H
#include <string>
#include <QPixmap>

class Host
{
public:
    Host(bool visible = true, std::string name = "none");
    bool isVisible();
    QPixmap getAvatar();

private:
    bool visible;
    std::string name;
    QPixmap* avatar;
    //address ip;
    //picture avatar;
};

#endif // HOST_H
