#include "PartSet_Module.h"

#include <Config_ModuleReader.h>

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
  Config_ModuleReader* aXMLReader = new Config_ModuleReader();
  aXMLReader->getModuleName();
  aXMLReader->setAutoImport(true);
  aXMLReader->readAll();
  delete aXMLReader;
}
