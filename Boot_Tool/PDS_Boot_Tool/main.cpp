#include <QCoreApplication>
#include <QFileInfo>
#include "JlCompress.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    QString path = "C:/Users/Benjamin/Desktop/screenShots_Utils";

    for (int i = 1; i < argc; ++i){
        path.append(QString(argv[i]));
        if(i < argc - 1){
            path.append(" ");
        }
    }

    // Understand if path is directory or file
    QFileInfo outputPath(path);

    if(outputPath.isDir()){
        // Send path to main app
        qDebug() << "Starting";
        QString zipFile("./compressedDir.zip");
        if(JlCompress::compressDir(zipFile, path)){
            qDebug() << "Complete";
        }
        else {
            qDebug() << "Error";
        }
    }
    else {
        // ZIP directory and send path of zipped file to main app

    }


    return 0;// a.exec();
}
