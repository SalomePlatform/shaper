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
#include <XGUI_ViewPort.h>

#include <Config_PointerMessage.h>
#include <Config_ModuleReader.h>
#include <Config_WidgetReader.h>
#include <Events_Loop.h>
#include <Events_Message.h>

#include <GeomAPI_Shape.h>

#include <AIS_ListOfInteractive.hxx>

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
  myListener = new PartSet_Listener(this);

  XGUI_OperationMgr* anOperationMgr = myWorkshop->operationMgr();

  connect(anOperationMgr, SIGNAL(operationStarted()), this, SLOT(onOperationStarted()));
  connect(anOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)),
          this, SLOT(onOperationStopped(ModuleBase_Operation*)));
  if (!myWorkshop->isSalomeMode()) {
    connect(myWorkshop->mainWindow()->viewer(), SIGNAL(mouseReleased(QPoint)),
            this, SLOT(onMouseReleased(QPoint)));
    connect(myWorkshop->mainWindow()->viewer(), SIGNAL(mouseMoved(QPoint)),
            this, SLOT(onMouseMoved(QPoint)));
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
    connect(aPreviewOp, SIGNAL(viewerProjectionChange(double, double, double)),
            this, SLOT(onViewerProjectionChange(double, double, double)));
  }
}

void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
{
  ModuleBase_PropPanelOperation* anOperation = dynamic_cast<ModuleBase_PropPanelOperation*>(theOperation);
  if (!anOperation)
    return;
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp)
    visualizePreview(false);
}

void PartSet_Module::onMouseReleased(QPoint thePoint)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
    if (aViewer) {
      NCollection_List<TopoDS_Shape> aList;
      aViewer->getSelectedShapes(aList);
      XGUI_ViewWindow* aWindow = aViewer->activeViewWindow();
      if (aWindow) {
        Handle(V3d_View) aView3d = aWindow->viewPort()->getView();
        if ( !aView3d.IsNull() ) {
          gp_Pnt aPoint = PartSet_Tools::ConvertClickToPoint(thePoint, aView3d);
          aPreviewOp->setSelectedShapes(aList, aPoint);
        }
      }
    }
  }
}

void PartSet_Module::onMouseMoved(QPoint thePoint)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
    if (aViewer) {
      XGUI_ViewWindow* aWindow = aViewer->activeViewWindow();
      if (aWindow) {
        Handle(V3d_View) aView3d = aWindow->viewPort()->getView();
        if ( !aView3d.IsNull() ) {
          gp_Pnt aPoint = PartSet_Tools::ConvertClickToPoint(thePoint, aView3d);
          aPreviewOp->setMouseMovePoint(aPoint);
        }
      }
    }
  }
}

void PartSet_Module::onViewerProjectionChange(double theX, double theY, double theZ)
{
  XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
  if (aViewer) {
    aViewer->setViewProjection(theX, theY, theZ);
  }
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
      aDisplayer->LocalSelection(anOperation->feature(),
                                   aPreview->impl<TopoDS_Shape>(), aPreviewOp->getSelectionMode());
    }
  }
  else {
    aDisplayer->GlobalSelection(false);
    aDisplayer->Erase(anOperation->feature());
  }
}
