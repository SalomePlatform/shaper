
#include "ModuleBase_IModule.h"
#include "ModuleBase_IViewer.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_Operation.h"
#include "ModuleBase_ISelection.h"

#include <Events_Loop.h>

#include <ModelAPI_Events.h>

#include <Config_PointerMessage.h>


ModuleBase_IModule::ModuleBase_IModule(ModuleBase_IWorkshop* theParent)
  : QObject(theParent), myWorkshop(theParent) 
{
  connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
  connect(myWorkshop->viewer(), SIGNAL(mousePress(QMouseEvent*)), this,
          SLOT(onMousePressed(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(mouseRelease(QMouseEvent*)), this,
          SLOT(onMouseReleased(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(mouseMove(QMouseEvent*)), this,
          SLOT(onMouseMoved(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(keyRelease(QKeyEvent*)), this,
          SLOT(onKeyRelease(QKeyEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(mouseDoubleClick(QMouseEvent*)), this,
          SLOT(onMouseDoubleClick(QMouseEvent*)));
}


void ModuleBase_IModule::launchOperation(const QString& theCmdId)
{
  ModuleBase_Operation* anOperation = createOperation(theCmdId.toStdString());
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  // Initialise operation with preliminary selection
  QList<ModuleBase_ViewerPrs> aSelected = aSelection->getSelected();
  QList<ModuleBase_ViewerPrs> aHighlighted = aSelection->getHighlighted();
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
