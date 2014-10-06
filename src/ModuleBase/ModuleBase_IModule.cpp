
#include "ModuleBase_IModule.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_Operation.h"
#include "ModuleBase_ISelection.h"

#include <Events_Loop.h>

#include <ModelAPI_Events.h>

#include <Config_PointerMessage.h>


void ModuleBase_IModule::launchOperation(const QString& theCmdId)
{
  ModuleBase_Operation* anOperation = createOperation(theCmdId.toStdString());
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  // Initialise operation with preliminary selection
  std::list<ModuleBase_ViewerPrs> aSelected = aSelection->getSelected();
  std::list<ModuleBase_ViewerPrs> aHighlighted = aSelection->getHighlighted();
  anOperation->initSelection(aSelected, aHighlighted);
  sendOperation(anOperation);
}


void ModuleBase_IModule::sendOperation(ModuleBase_Operation* theOperation)
{
  static Events_ID aModuleEvent = Events_Loop::eventByName(EVENT_OPERATION_LAUNCHED);
  boost::shared_ptr<Config_PointerMessage> aMessage =
      boost::shared_ptr<Config_PointerMessage>(new Config_PointerMessage(aModuleEvent, this));
  aMessage->setPointer(theOperation);
  Events_Loop::loop()->send(aMessage);
}
