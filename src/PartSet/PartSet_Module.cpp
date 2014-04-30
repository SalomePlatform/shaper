#include <PartSet_Module.h>
#include <PartSet_OperationSketch.h>
#include <PartSet_OperationSketchLine.h>
#include <PartSet_Listener.h>
#include <PartSet_Tools.h>

#include <ModuleBase_Operation.h>

#include <XGUI_MainWindow.h>
#include <XGUI_Displayer.h>
#include <XGUI_Viewer.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_ViewWindow.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_ViewPort.h>
#include <XGUI_ActionsMgr.h>

#include <Config_PointerMessage.h>
#include <Config_ModuleReader.h>
#include <Config_WidgetReader.h>
#include <Events_Loop.h>
#include <Events_Message.h>
#include <Events_Error.h>

#include <GeomAPI_Shape.h>

#include <AIS_ListOfInteractive.hxx>

#include <QObject>
#include <QMouseEvent>
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
  myListener = new PartSet_Listener(this);

  XGUI_OperationMgr* anOperationMgr = myWorkshop->operationMgr();

  connect(anOperationMgr, SIGNAL(operationStarted()), this, SLOT(onOperationStarted()));
  connect(anOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)),
          this, SLOT(onOperationStopped(ModuleBase_Operation*)));
  if (!myWorkshop->isSalomeMode()) {
    XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
    connect(aViewer, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(aViewer, SIGNAL(mouseRelease(XGUI_ViewWindow*, QMouseEvent*)),
            this, SLOT(onMouseReleased(XGUI_ViewWindow*, QMouseEvent*)));
    connect(aViewer, SIGNAL(mouseMove(XGUI_ViewWindow*, QMouseEvent*)),
            this, SLOT(onMouseMoved(XGUI_ViewWindow*, QMouseEvent*)));
  }
}

PartSet_Module::~PartSet_Module()
{
}

void PartSet_Module::createFeatures()
{
  Config_ModuleReader aXMLReader = Config_ModuleReader();
  aXMLReader.readAll();
  myFeaturesInFiles = aXMLReader.featuresInFiles();
}

void PartSet_Module::featureCreated(XGUI_Command* theFeature)
{
  theFeature->connectTo(this, SLOT(onFeatureTriggered()));
}

QStringList PartSet_Module::nestedFeatures(QString)
{
  return QStringList();
}

std::string PartSet_Module::featureFile(const std::string& theFeatureId)
{
  return myFeaturesInFiles[theFeatureId];
}

/*
 *
 */
void PartSet_Module::onFeatureTriggered()
{
  XGUI_Command* aCmd = dynamic_cast<XGUI_Command*>(sender());
  //Do nothing on uncheck
  if(aCmd->isCheckable() && !aCmd->isChecked())
    return;
  launchOperation(aCmd->id());
}
  
void PartSet_Module::launchOperation(const QString& theCmdId)
{
  std::string aStdCmdId = theCmdId.toStdString();
  std::string aPluginFileName = featureFile(aStdCmdId);
  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginFileName);
  aWdgReader.readAll();
  std::string aXmlCfg = aWdgReader.featureWidgetCfg(aStdCmdId);
  std::string aDescription = aWdgReader.featureDescription(aStdCmdId);
  ModuleBase_PropPanelOperation* aPartSetOp;
  if (theCmdId == "Sketch" ) {
    aPartSetOp = new PartSet_OperationSketch(theCmdId, this);
  }
  else if(theCmdId == "SketchLine") {
    ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
    boost::shared_ptr<ModelAPI_Feature> aSketchFeature;
    if (anOperation)
      aSketchFeature = anOperation->feature();
    aPartSetOp = new PartSet_OperationSketchLine(theCmdId, this, aSketchFeature);
  }
  else {
    aPartSetOp = new ModuleBase_PropPanelOperation(theCmdId, this);
  }
  aPartSetOp->setXmlRepresentation(QString::fromStdString(aXmlCfg));
  aPartSetOp->setDescription(QString::fromStdString(aDescription));

  //TODO(sbh): Implement static method to extract event id [SEID]
  static Events_ID aModuleEvent = Events_Loop::eventByName("PartSetModuleEvent");
  Config_PointerMessage aMessage(aModuleEvent, this);
  aMessage.setPointer(aPartSetOp);
  Events_Loop::loop()->send(aMessage);
}

void PartSet_Module::onOperationStarted()
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();

  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    visualizePreview(true);

    PartSet_OperationSketch* aSketchOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
    if (aSketchOp) {
      connect(aSketchOp, SIGNAL(planeSelected(double, double, double)),
              this, SLOT(onPlaneSelected(double, double, double)));
    }
  }
}

void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
{
  ModuleBase_PropPanelOperation* anOperation = dynamic_cast<ModuleBase_PropPanelOperation*>(theOperation);
  if (!anOperation)
    return;
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  //if (aPreviewOp)
  //  visualizePreview(false);
}

void PartSet_Module::onSelectionChanged()
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    XGUI_SelectionMgr* aSelector = myWorkshop->selector();
    if (aSelector) {
      NCollection_List<TopoDS_Shape> aList;
      aSelector->selectedShapes(aList);
      aPreviewOp->setSelectedShapes(aList);
    }
  }
}

void PartSet_Module::onMouseReleased(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent)
{
  QPoint aPoint = theEvent->pos();
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    XGUI_SelectionMgr* aSelector = myWorkshop->selector();
    if (aSelector) {
      XGUI_ViewWindow* aWindow = myWorkshop->mainWindow()->viewer()->activeViewWindow();
      if (aWindow) {
        Handle(V3d_View) aView3d = aWindow->viewPort()->getView();
        if ( !aView3d.IsNull() ) {
          gp_Pnt aPnt = PartSet_Tools::ConvertClickToPoint(aPoint, aView3d);
          aPreviewOp->mouseReleased(aPnt);
        }
      }
    }
  }
}

void PartSet_Module::onMouseMoved(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent)
{
  QPoint aPoint = theEvent->pos();
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
    if (aViewer) {
      XGUI_ViewWindow* aWindow = aViewer->activeViewWindow();
      if (aWindow) {
        Handle(V3d_View) aView3d = aWindow->viewPort()->getView();
        if ( !aView3d.IsNull() ) {
          gp_Pnt aPnt = PartSet_Tools::ConvertClickToPoint(aPoint, aView3d);
          aPreviewOp->mouseMoved(aPnt);
        }
      }
    }
  }
}

void PartSet_Module::onPlaneSelected(double theX, double theY, double theZ)
{
  XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
  if (aViewer) {
    aViewer->setViewProjection(theX, theY, theZ);
  }

  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  if (anOperation) {
    PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
    if (aPreviewOp) {
      aPreviewOp->setEditMode(true);
      visualizePreview(false);
    }
  }

  myWorkshop->actionsMgr()->setNestedActionsEnabled(true);
}

void PartSet_Module::visualizePreview(bool isDisplay)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  if (!anOperation)
    return;

  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (!aPreviewOp)
    return;

  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  if (isDisplay) {
    boost::shared_ptr<GeomAPI_Shape> aPreview = aPreviewOp->preview();
    if (aPreview) {
      aDisplayer->DisplayInLocalContext(anOperation->feature(), aPreview->impl<TopoDS_Shape>(),
                                        aPreviewOp->getSelectionMode());
    }
  }
  else {
    aDisplayer->CloseLocalContexts(false);
    aDisplayer->Erase(anOperation->feature());
  }
}
