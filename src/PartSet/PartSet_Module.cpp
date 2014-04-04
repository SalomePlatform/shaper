#include <PartSet_Module.h>
#include <Config_WidgetMessage.h>

#include <Config_ModuleReader.h>
#include <Config_WidgetReader.h>
#include <Event_Loop.h>
#include <Event_Message.h>

#include <QObject>
#include <QString>

#ifdef _DEBUG
#include <QDebug>
#endif


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
  theFeature->connectTo(this, SLOT(onCommandTriggered()));
}

void PartSet_Module::onCommandTriggered()
{
  Config_ModuleReader aModuleReader = Config_ModuleReader();
  aModuleReader.readAll();
  std::map<std::string, std::string> aPluginMap = aModuleReader.plugins();
  std::string aPluginName = aPluginMap["PartSetPlugin"];
  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginName);
  aWdgReader.readAll();
  XGUI_Command* aCmd = dynamic_cast<XGUI_Command*>(sender());
  std::string aCmdId = aCmd->getId().toStdString();
  std::string aXMLWidgetCfg = aWdgReader.featureWidgetCfg(aCmdId);
  //TODO(sbh): Implement static method to extract event id [SEID]
  static Event_ID aModuleEvent = Event_Loop::eventByName("PartSetModuleEvent");
  Config_WidgetMessage aMessage(aModuleEvent, this);
  aMessage.setFeatureId(aCmdId);
  aMessage.setXmlRepresentation(aXMLWidgetCfg);
  Event_Loop::loop()->send(aMessage);
}
