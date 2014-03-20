#include "PartSet_Module.h"

#include <Config_XMLReader.h>

#include <QFile>
#include <QDir>
#include <QApplication>
#include <QTextStream>

/*!Create and return new instance of XGUI_Module*/
extern "C" PARTSET_EXPORT XGUI_Module* createModule(XGUI_Workshop* theWshop)
{
  return new PartSet_Module(theWshop);
}


PartSet_Module::PartSet_Module(XGUI_Workshop* theWshop)
{
    myWorkshop = theWshop;
}


PartSet_Module::~PartSet_Module()
{
}


void PartSet_Module::createFeatures()
{
  Config_XMLReader* aReader =
          new Config_XMLReader("plugin-PartSet.xml");
  aReader->readAll();
}
