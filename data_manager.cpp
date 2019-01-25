#include "data_manager.h"

Data_Manager::Data_Manager(QString localHostName)
{
    bool visible = false;
    localHost = new Host(visible, localHostName);
    localHost->createuniqueID();
}

void Data_Manager::addOnlineUser(Host newHost){
    onlineUsers.push_back(newHost);
    emit isUpdated();
}

void Data_Manager::deleteOnlineUser(Host hostToDelete){
    // Comparison is done with the unique ID (operator overload == in 'Host' class)
    onlineUsers.remove(hostToDelete);
    emit isUpdated();
}


std::list<Host> Data_Manager::getOnlineUsers(){
    return onlineUsers;
}

bool Data_Manager::isPresentInOnlineUsers(QUuid id){
    for(std::list<Host>::iterator it = onlineUsers.begin(); it != onlineUsers.end(); it++){
        if(it->getUniqueID() == id){
            return true;
        }
    }
    return false;
}

void Data_Manager::addToSendUsers(QUuid uniqueID){
    for(std::list<Host>::iterator it = onlineUsers.begin(); it != onlineUsers.end(); it++){
        if(it->getUniqueID() == uniqueID){
            toSend.push_back(*it);
        }
    }
}

void Data_Manager::deleteToSendUsers(QUuid uniqueID){

    toSend.remove_if([uniqueID](Host h){
        if(h.getUniqueID() == uniqueID){
            return true;
        } else {
            return false;
        }
    });
}

void Data_Manager::DEBUG_clearOnlineUsers(){
    onlineUsers.clear();
    emit isUpdated();
}
