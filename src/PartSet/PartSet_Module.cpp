#include "PartSet_Module.h"

#include <Config_ModuleReader.h>
#include <Config_FeatureReader.h>

#include <Event_Loop.h>

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
  aXMLReader->setAutoImport(true);
  aXMLReader->readAll();
  delete aXMLReader;
}

void PartSet_Module::featureCreated(XGUI_Command* theFeature)
{
  QString aFtId = theFeature->getId();
  if (aFtId == "new_point") {
    theFeature->connectTo(this, SLOT(onCommandTriggered()));
  }
}

void PartSet_Module::onCommandTriggered()
{
  Config_ModuleReader aModuleReader = Config_ModuleReader();
  aModuleReader.readAll();
  std::string aPluginName = aModuleReader.pluginsList().front();
  Config_FeatureReader* aReader = new Config_FeatureReader(aPluginName);
  XGUI_Command* aCmd = dynamic_cast<XGUI_Command*>(sender());
  std::string aXMLWidgetCfg = aReader->featureWidgetCfg(aCmd->getId().toStdString());
  delete aReader;
}
