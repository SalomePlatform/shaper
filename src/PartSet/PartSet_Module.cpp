#include <PartSet_Module.h>
#include <PartSet_OperationSketch.h>
#include <PartSet_OperationSketchLine.h>
#include <PartSet_OperationEditLine.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <PartSet_Listener.h>
#include <PartSet_Tools.h>

#include <ModuleBase_Operation.h>

#include <XGUI_MainWindow.h>
#include <XGUI_Displayer.h>
#include <XGUI_Viewer.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_ViewPort.h>
#include <XGUI_ActionsMgr.h>
#include <XGUI_ViewerProxy.h>

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

  connect(myWorkshop->selector(), SIGNAL(selectionChanged()), 
          this, SLOT(onSelectionChanged()));
  connect(myWorkshop->viewer(), SIGNAL(mouseRelease(QMouseEvent*)),
          this, SLOT(onMouseReleased(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(mouseMove(QMouseEvent*)),
          this, SLOT(onMouseMoved(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(keyRelease(QKeyEvent*)),
          this, SLOT(onKeyRelease(QKeyEvent*)));
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
  ModuleBase_Operation* anOperation = createOperation(theCmdId);
  sendOperation(anOperation);
}

void PartSet_Module::onOperationStarted()
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();

  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    visualizePreview(aPreviewOp->feature(), true);

    connect(aPreviewOp, SIGNAL(featureConstructed(boost::shared_ptr<ModelAPI_Feature>, int)),
            this, SLOT(onFeatureConstructed(boost::shared_ptr<ModelAPI_Feature>, int)));

    PartSet_OperationSketch* aSketchOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
    if (aSketchOp) {
      connect(aSketchOp, SIGNAL(planeSelected(double, double, double)),
              this, SLOT(onPlaneSelected(double, double, double)));
      connect(aSketchOp, SIGNAL(launchOperation(std::string, boost::shared_ptr<ModelAPI_Feature>)),
              this, SLOT(onLaunchOperation(std::string, boost::shared_ptr<ModelAPI_Feature>)));

    }
  }
}

void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
{
  if (!theOperation)
    return;
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(theOperation);
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

      XGUI_Displayer* aDisplayer = myWorkshop->displayer();
      boost::shared_ptr<ModelAPI_Feature> aFeature;
      if (!aList.IsEmpty()) {
        const TopoDS_Shape& aShape = aList.First();
        aFeature = aDisplayer->GetFeature(aShape);
        aPreviewOp->setSelected(aFeature, aShape);
      }
    }
  }
}

void PartSet_Module::onMouseReleased(QMouseEvent* theEvent)
{
  QPoint aPoint = theEvent->pos();
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    XGUI_SelectionMgr* aSelector = myWorkshop->selector();
    if (aSelector) {
      Handle(V3d_View) aView3d = myWorkshop->viewer()->activeView();
      if ( !aView3d.IsNull() ) {
        gp_Pnt aPnt = PartSet_Tools::ConvertClickToPoint(aPoint, aView3d);
        aPreviewOp->mouseReleased(aPnt);
      }
    }
  }
}

void PartSet_Module::onMouseMoved(QMouseEvent* theEvent)
{
  QPoint aPoint = theEvent->pos();
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    Handle(V3d_View) aView3d = myWorkshop->viewer()->activeView();
    if ( !aView3d.IsNull() ) {
      gp_Pnt aPnt = PartSet_Tools::ConvertClickToPoint(aPoint, aView3d);
      aPreviewOp->mouseMoved(aPnt);
    }
  }
}

void PartSet_Module::onKeyRelease(QKeyEvent* theEvent)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    aPreviewOp->keyReleased(theEvent->key());
  }
}

void PartSet_Module::onPlaneSelected(double theX, double theY, double theZ)
{
  myWorkshop->viewer()->setViewProjection(theX, theY, theZ);

  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  if (anOperation) {
    PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
    if (aPreviewOp) {
      visualizePreview(aPreviewOp->feature(), false);
    }
  }

  myWorkshop->actionsMgr()->setNestedActionsEnabled(true);
}

void PartSet_Module::onLaunchOperation(std::string theName, boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  ModuleBase_Operation* anOperation = createOperation(theName.c_str());
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp)
  {
    aPreviewOp->init(theFeature);
  }
  sendOperation(anOperation);
}

void PartSet_Module::onFeatureConstructed(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                          int theMode)
{
  bool isDisplay = theMode != PartSet_OperationSketchBase::FM_Abort;
  visualizePreview(theFeature, isDisplay);
}

ModuleBase_Operation* PartSet_Module::createOperation(const QString& theCmdId)
{
  std::string aStdCmdId = theCmdId.toStdString();
  if (aStdCmdId == "EditLine")
    aStdCmdId = "SketchLine";
  std::string aPluginFileName = featureFile(aStdCmdId);
  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginFileName);
  aWdgReader.readAll();
  std::string aXmlCfg = aWdgReader.featureWidgetCfg(aStdCmdId);
  std::string aDescription = aWdgReader.featureDescription(aStdCmdId);
  ModuleBase_Operation* anOperation;
  if (theCmdId == "Sketch" ) {
    anOperation = new PartSet_OperationSketch(theCmdId, this);
  }
  else if(theCmdId == "SketchLine" || theCmdId == "EditLine") {
    ModuleBase_Operation* aCurOperation = myWorkshop->operationMgr()->currentOperation();
    boost::shared_ptr<ModelAPI_Feature> aSketchFeature;
    if (aCurOperation)
      aSketchFeature = aCurOperation->feature();
    if (theCmdId == "SketchLine")
      anOperation = new PartSet_OperationSketchLine(theCmdId, this, aSketchFeature);
    else
      anOperation = new PartSet_OperationEditLine(theCmdId, this, aSketchFeature);
  }
  else {
    anOperation = new ModuleBase_Operation(theCmdId, this);
  }
  anOperation->getDescription()->setXmlRepresentation(QString::fromStdString(aXmlCfg));
  anOperation->getDescription()->setDescription(QString::fromStdString(aDescription));

  return anOperation;
}

void PartSet_Module::sendOperation(ModuleBase_Operation* theOperation)
{
  //TODO(sbh): Implement static method to extract event id [SEID]
  static Events_ID aModuleEvent = Events_Loop::eventByName("PartSetModuleEvent");
  Config_PointerMessage aMessage(aModuleEvent, this);
  aMessage.setPointer(theOperation);
  Events_Loop::loop()->send(aMessage);
}

void PartSet_Module::visualizePreview(boost::shared_ptr<ModelAPI_Feature> theFeature, bool isDisplay)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  if (!anOperation)
    return;

  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (!aPreviewOp)
    return;

  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  if (isDisplay) {
    boost::shared_ptr<GeomAPI_Shape> aPreview = aPreviewOp->preview(theFeature);
    if (aPreview) {
      aDisplayer->RedisplayInLocalContext(theFeature, aPreview->impl<TopoDS_Shape>(),
                                          aPreviewOp->getSelectionModes(theFeature));
    }
  }
  else {
    //aDisplayer->CloseLocalContexts(false);
    aDisplayer->Erase(anOperation->feature());
  }
}
