#include "data_manager.h"

Data_Manager::Data_Manager(QString localHostName, QString filePath)
{
    bool visible = true;
    localHost = new Host(visible, localHostName);
    localHost->createuniqueID();

    if(filePath !=nullptr){
        this->filePath = filePath;
        QStringList tokens;
        tokens = filePath.split("/");
        fileName = tokens.last();
        qDebug() << "DataManager: FileName : " << fileName;
    }
}

void Data_Manager::addOnlineUser(Host newHost){ //called by setAvatarOfNextOnlineUser, that has already a mutex
    onlineUsers.push_back(newHost);
    emit isUpdated();
}

void Data_Manager::deleteOnlineUser(QUuid hostID){
    // Comparison is done with the unique ID (operator overload == in 'Host' class)
    mutex.lock();
    qDebug() << "DELETE entered....";

    std::list<Host>::iterator it;

    for(it = onlineUsers.begin(); it != onlineUsers.end(); it++){

        if(it->getUniqueID() == hostID)
            it = onlineUsers.erase(it);

    }
    mutex.unlock();
    emit isUpdated();

}


std::list<Host> Data_Manager::getOnlineUsers(){
    mutex.lock();
    std::list<Host> tmp = onlineUsers;
    mutex.unlock();
    return tmp;
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

bool Data_Manager::isPresentInToSendUsers(QUuid id){
    if(!toSend.empty()){
        for(std::list<Host>::iterator it = toSend.begin(); it != toSend.end(); it++){
            if(it->getUniqueID() == id){
                return true;
            }
        }
    }
    return false;
}

void Data_Manager::addToSendUser(QUuid uniqueID){
    mutex.lock(); //used for onlineUser lock access
    for(std::list<Host>::iterator it = onlineUsers.begin(); it != onlineUsers.end(); it++){
        if(!isPresentInToSendUsers(uniqueID) && it->getUniqueID() == uniqueID){
            toSend.push_back(*it);
        }
    }
    mutex.unlock();
}

void Data_Manager::deleteToSendUser(QUuid uniqueID){

    toSend.remove_if([uniqueID](Host h){
        if(h.getUniqueID() == uniqueID){
            return true;
        } else {
            return false;
        }
    });
}

void Data_Manager::deleteAllToSendUsers(){

    toSend.clear();
    emit isUpdated();
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
            it = queueNextOnlineUsers.erase(it);
        }
    }
    mutex.unlock();
}

QString Data_Manager::getFilePath(){
    return filePath;
}

QString Data_Manager::getFileName(){
    return fileName;
}

void Data_Manager::setFilePath(QString path, bool isDir){
    this->isDir = isDir;
    path.replace("\\", "/");

    if(filePath !=nullptr){
        this->filePath = path;

        QStringList tokens;

        tokens = this->filePath.split("/");

        this->fileName = tokens.last();
        qDebug() << "DataManager: FileName : " << fileName;

        emit sendNewFile(fileName);
    }
}

bool Data_Manager::getIsDir(){
    return this->isDir;
}

void Data_Manager::refreshOnlineUsers(){
    time_t currentTime = time(nullptr);

    qDebug() << "TIME entered....";

    std::list<Host>::iterator it;
    bool isModified = false;

    mutex.lock();

    for(it = onlineUsers.begin(); it != onlineUsers.end(); it++){
        qDebug() << "TIME in refresh online = " << difftime(currentTime, it->getLastSeen());

        if(difftime(currentTime, it->getLastSeen()) > refreshTime*2){
            //deleteOnlineUser(*it);
            it = onlineUsers.erase(it);
            isModified = true;
        }
    }

    mutex.unlock();

    if(isModified)
        emit isUpdated();
}

uint Data_Manager::getRefreshTime(){
    return refreshTime;
}

void Data_Manager::updateHostInfo(QUuid uniqueID, time_t time, QString name){
    mutex.lock();
    for(Host &tmp : onlineUsers){
        if(tmp.getUniqueID() == uniqueID){
            tmp.setLastSeen(time);
            if(tmp.getName() != name){
                tmp.setName(name);
                mutex.unlock();
                emit isUpdated();
                return;
            }
        }
    }
    mutex.unlock();
}


void Data_Manager::setLocalHostVisibility(bool visibility){
    if(localHost->isVisible() == true && visibility == false){
        emit quittingApplication();
    }
    if(localHost->isVisible() != visibility){
        localHost->setVisibility(visibility);
        emit isUpdated();
    }

}

void Data_Manager::setLocalHostName(QString name){
    localHost->setName(name);
}

void Data_Manager::setReceiveFilesAutom(bool autom){
    this->receiveFileAutomatically = autom;
}

bool Data_Manager::getReceiveFilesAutom(){
    return this->receiveFileAutomatically;
}

void Data_Manager::setIFDefaultSavingPath(bool defaultSave){
    this->defaultSavingPath = defaultSave;
}

bool Data_Manager::getIFDefaultSavingPath(){
    return this->defaultSavingPath;
}

void Data_Manager::DEBUG_clearOnlineUsers(){
    onlineUsers.clear();
    emit isUpdated();
}

void Data_Manager::DEBUG_trySlot(QUuid id){
    qDebug() << "TRY" << id;
}
