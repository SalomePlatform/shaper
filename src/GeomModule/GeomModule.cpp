#include "GeomModule.h"

#include <Config_XMLReader.h>

#include <QFile>
#include <QDir>
#include <QApplication>
#include <QTextStream>

/*!Create and return new instance of XGUI_Module*/
extern "C" GM_EXPORT IModule* createModule(IWorkshop* theWshop)
{
  return new GeomModule(theWshop);
}


GeomModule::GeomModule(IWorkshop* theWshop)
{
    myWorkshop = theWshop;
}


GeomModule::~GeomModule()
{
}


void GeomModule::createFeatures()
{
  Config_XMLReader* aReader =
          new Config_XMLReader("plugin-PartSet.xml");
  aReader->readAll();
}
