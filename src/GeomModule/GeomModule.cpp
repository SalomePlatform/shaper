#include "GeomModule.h"

#include <QFile>
#include <QDir>
#include <QApplication>
#include <QTextStream>

/*!Create and return new instance of XGUI_Module*/
extern "C" GM_EXPORT XGUI_Module* createModule()
{
  return new GeomModule();
}


GeomModule::GeomModule()
{
    QString aDir = qApp->applicationDirPath();
    QString aXMLFile = aDir + QDir::separator() + "main_menu.xml";

    QFile aFile(aXMLFile);
    if (aFile.open((QIODevice::ReadOnly | QIODevice::Text))) {
        QTextStream aTextStream(&aFile);
        myMenuXML = aTextStream.readAll();
    }
}


GeomModule::~GeomModule()
{
}

QString GeomModule::moduleDescription() const
{
    return myMenuXML;
}