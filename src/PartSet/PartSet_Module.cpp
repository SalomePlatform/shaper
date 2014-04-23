#include <PartSet_Module.h>
#include <ModuleBase_PropPanelOperation.h>

#include <Config_PointerMessage.h>
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
  Config_ModuleReader aXMLReader = Config_ModuleReader();
  aXMLReader.setAutoImport(true);
  aXMLReader.readAll();
}

void PartSet_Module::featureCreated(XGUI_Command* theFeature)
{
  theFeature->connectTo(this, SLOT(onFeatureTriggered()));
}

std::string PartSet_Module::modulePlugin()
{
  Config_ModuleReader aModuleReader = Config_ModuleReader();
  aModuleReader.readAll();
  std::map < std::string, std::string > aPluginMap = aModuleReader.plugins();
  std::string aPluginName = aPluginMap["PartSetPlugin"];
  return aPluginName;
}

/*
 *
 */
void PartSet_Module::onFeatureTriggered()
{
  std::string aPluginName = modulePlugin();
  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginName);
  aWdgReader.readAll();
  XGUI_Command* aCmd = dynamic_cast<XGUI_Command*>(sender());
  QString aCmdId = aCmd->id();
  std::string aXmlCfg = aWdgReader.featureWidgetCfg(aCmdId.toStdString());
  std::string aDescription = aWdgReader.featureDescription(aCmdId.toStdString());

  ModuleBase_PropPanelOperation* aPartSetOp = new ModuleBase_PropPanelOperation(aCmdId, this);
  aPartSetOp->setXmlRepresentation(QString::fromStdString(aXmlCfg));
  aPartSetOp->setDescription(QString::fromStdString(aDescription));

  //TODO(sbh): Implement static method to extract event id [SEID]
  static Event_ID aModuleEvent = Event_Loop::eventByName("PartSetModuleEvent");
  Config_PointerMessage aMessage(aModuleEvent, this);
  aMessage.setPointer(aPartSetOp);
  Event_Loop::loop()->send(aMessage);
}
