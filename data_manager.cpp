#include "data_manager.h"

Data_Manager::Data_Manager(QString localHostName, QString filePath)
{
    bool visible = true;
    localHost = new Host(visible, localHostName);
    localHost->createuniqueID();

    if(filePath !=nullptr){
        fileToSend = new QFile(filePath);
        QStringList tokens;
        tokens = filePath.split("/");
        fileName = tokens.last();
        qDebug() << "DataManager: FileName : " << fileName;
    }
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

std::list<Host> Data_Manager::getToSendUsers(){
    return toSend;
}

bool Data_Manager::isPresentInOnlineUsers(QUuid id){
    mutex.lock();

    if(!queueNextOnlineUsers.empty()){
        for(std::list<Host>::iterator it = queueNextOnlineUsers.begin(); it != queueNextOnlineUsers.end(); it++){
            if(it->getUniqueID() == id){
                mutex.unlock();
                return true;
            }
        }
    }

    if(!onlineUsers.empty()){
        for(std::list<Host>::iterator it = onlineUsers.begin(); it != onlineUsers.end(); it++){
            if(it->getUniqueID() == id){
                mutex.unlock();
                return true;
            }
        }
    }

    mutex.unlock();
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

void Data_Manager::addQueueNextOnlineUsers(Host h){
    mutex.lock();
    queueNextOnlineUsers.push_back(h);
    mutex.unlock();
}

void Data_Manager::setAvatarOfNextOnlineUser(QPixmap avatar, QUuid uniqueID){
    mutex.lock();
    for(std::list<Host>::iterator it = queueNextOnlineUsers.begin(); it != queueNextOnlineUsers.end(); it++){
        if(it->getUniqueID() == uniqueID){
            it->setAvatar(avatar);
            addOnlineUser(*it);
        }
    }
    mutex.unlock();
}

QFile* Data_Manager::getFileToSend(){
    return fileToSend;
}

QString Data_Manager::getFileName(){
    return fileName;
}

void Data_Manager::DEBUG_clearOnlineUsers(){
    onlineUsers.clear();
    emit isUpdated();
}

void Data_Manager::DEBUG_trySlot(QUuid id, qint64 max){
    qDebug() << "TRY" << id << max;
}
