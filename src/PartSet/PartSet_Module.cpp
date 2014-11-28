#include "PartSet_Module.h"
//#include <PartSet_OperationSketch.h>
//#include <PartSet_OperationFeatureCreate.h>
//#include <PartSet_OperationFeatureEdit.h>
//#include <PartSet_Listener.h>
//#include <PartSet_TestOCC.h>
#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_Validators.h"
#include "PartSet_Tools.h"
#include "PartSet_WidgetPoint2D.h"
#include "PartSet_WidgetPoint2dDistance.h"
#include "PartSet_WidgetShapeSelector.h"

#include <ModuleBase_Operation.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IPropertyPanel.h>
//#include <ModuleBase_OperationDescription.h>
//#include <ModuleBase_WidgetFactory.h>

#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>

#include <XGUI_MainWindow.h>
#include <XGUI_Displayer.h>
#include <XGUI_Viewer.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_ViewPort.h>
#include <XGUI_ActionsMgr.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_ContextMenuMgr.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_Tools.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>

//#include <Config_PointerMessage.h>
//#include <Config_ModuleReader.h>
//#include <Config_WidgetReader.h>
//#include <Events_Loop.h>
//#include <Events_Message.h>
//#include <Events_Error.h>

//#include <GeomAPI_Shape.h>
//#include <GeomAPI_AISObject.h>
//#include <AIS_Shape.hxx>
//#include <AIS_DimensionSelectionMode.hxx>

#include <StdSelect_TypeOfFace.hxx>

#include <QObject>
#include <QMouseEvent>
#include <QString>
#include <QTimer>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Dir.h>

#ifdef _DEBUG
#include <QDebug>
#endif


/*!Create and return new instance of XGUI_Module*/
extern "C" PARTSET_EXPORT ModuleBase_IModule* createModule(ModuleBase_IWorkshop* theWshop)
{
  return new PartSet_Module(theWshop);
}

PartSet_Module::PartSet_Module(ModuleBase_IWorkshop* theWshop)
  : ModuleBase_IModule(theWshop), 
  myIsDragging(false), myRestartingMode(true), myDragDone(false)
{
  //myWorkshop = dynamic_cast<XGUI_Workshop*>(theWshop);
  ModuleBase_IViewer* aViewer = aViewer = theWshop->viewer();
  connect(aViewer, SIGNAL(mousePress(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMousePressed(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseReleased(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseMoved(ModuleBase_IViewWindow*, QMouseEvent*)));

  //myListener = new PartSet_Listener(this);

//  connect(myWorkshop, SIGNAL(operationStarted(ModuleBase_Operation*)), 
//    this, SLOT(onOperationStarted(ModuleBase_Operation*)));

//  connect(myWorkshop, SIGNAL(operationStopped(ModuleBase_Operation*)), this,
//          SLOT(onOperationStopped(ModuleBase_Operation*)));

  //XGUI_Workshop* aXWshop = xWorkshop();
  //XGUI_ContextMenuMgr* aContextMenuMgr = aXWshop->contextMenuMgr();
  //connect(aContextMenuMgr, SIGNAL(actionTriggered(const QString&, bool)), this,
  //        SLOT(onContextMenuCommand(const QString&, bool)));

}

PartSet_Module::~PartSet_Module()
{
}

void PartSet_Module::registerValidators()
{
  //Registering of validators
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("PartSet_DistanceValidator", new PartSet_DistanceValidator);
  aFactory->registerValidator("PartSet_LengthValidator", new PartSet_LengthValidator);
  aFactory->registerValidator("PartSet_PerpendicularValidator", new PartSet_PerpendicularValidator);
  aFactory->registerValidator("PartSet_ParallelValidator", new PartSet_ParallelValidator);
  aFactory->registerValidator("PartSet_RadiusValidator", new PartSet_RadiusValidator);
}


void PartSet_Module::onOperationComitted(ModuleBase_Operation* theOperation) 
{
  if (theOperation->isEditOperation())
    return;
  /// Restart sketcher operations automatically
  FeaturePtr aFeature = theOperation->feature();
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
  if (aSPFeature && myRestartingMode) {
    myLastOperationId = theOperation->id();
    myLastFeature = theOperation->feature();
    launchOperation(myLastOperationId);
  } else {
    breakOperationSequence();
  }
}

void PartSet_Module::breakOperationSequence()
{
  myLastOperationId = "";
  myLastFeature = FeaturePtr();
  myRestartingMode = false;

}

void PartSet_Module::onOperationAborted(ModuleBase_Operation* theOperation)
{
  breakOperationSequence();
}

void PartSet_Module::onOperationStarted(ModuleBase_Operation* theOperation)
{
  myRestartingMode = true;
  if (theOperation->id().toStdString() == SketchPlugin_Sketch::ID()) {
    // Display all sketcher sub-Objects
    myCurrentSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theOperation->feature());
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
    XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

    for (int i = 0; i < myCurrentSketch->numberOfSubs(); i++) {
      FeaturePtr aFeature = myCurrentSketch->subFeature(i);
      std::list<ResultPtr> aResults = aFeature->results();
      std::list<ResultPtr>::const_iterator aIt;
      for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
        aDisplayer->display((*aIt), false);
      }
      aDisplayer->display(aFeature);
    }
    // Hide sketcher result
    std::list<ResultPtr> aResults = myCurrentSketch->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      aDisplayer->erase((*aIt), false);
    }
    aDisplayer->erase(myCurrentSketch);
  }
}

void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
{
  if (theOperation->id().toStdString() == SketchPlugin_Sketch::ID()) {
    // Hide all sketcher sub-Objects
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
    XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

    for (int i = 0; i < myCurrentSketch->numberOfSubs(); i++) {
      FeaturePtr aFeature = myCurrentSketch->subFeature(i);
      std::list<ResultPtr> aResults = aFeature->results();
      std::list<ResultPtr>::const_iterator aIt;
      for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
        aDisplayer->erase((*aIt), false);
      }
      aDisplayer->erase(aFeature, false);
    }
    // Display sketcher result
    std::list<ResultPtr> aResults = myCurrentSketch->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      aDisplayer->display((*aIt), false);
    }
    aDisplayer->display(myCurrentSketch);
    
    myCurrentSketch = CompositeFeaturePtr();
  }
}



void PartSet_Module::propertyPanelDefined(ModuleBase_Operation* theOperation)
{
  if ((theOperation->id() == myLastOperationId) && myLastFeature) {
    ModuleBase_ModelWidget* aWgt = theOperation->propertyPanel()->activeWidget();
    if (theOperation->id().toStdString() == SketchPlugin_Line::ID()) {
      PartSet_WidgetPoint2D* aPnt2dWgt = dynamic_cast<PartSet_WidgetPoint2D*>(aWgt);
      if (aPnt2dWgt) {
        std::shared_ptr<ModelAPI_Data> aData = myLastFeature->data();
        std::shared_ptr<GeomDataAPI_Point2D> aPoint = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::END_ID()));
        if (aPoint) {
          aPnt2dWgt->setPoint(aPoint->x(), aPoint->y());
          PartSet_Tools::setConstraints(myCurrentSketch, theOperation->feature(), 
            SketchPlugin_Line::START_ID(), aPoint->x(), aPoint->y());
          theOperation->propertyPanel()->activateNextWidget(aPnt2dWgt);
        }
      }
    }
  }
}


void PartSet_Module::onSelectionChanged()
{
  // Activate edit operation for sketcher objects
  //ModuleBase_ISelection* aSelect = myWorkshop->selection();
  //QObjectPtrList aObjects = aSelect->selectedPresentations();
  //if (aObjects.size() == 1) {
  //  FeaturePtr aFeature = ModelAPI_Feature::feature(aObjects.first());
  //  if (aFeature) {
  //    std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
  //              std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
  //    if (aSPFeature) {
  //      editFeature(aSPFeature);
  //    }
  //  }
  //}
}

//ModuleBase_Operation* PartSet_Module::getNewOperation(const std::string& theFeatureId)
//{
//  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
//  if (aOperation) {
//    // If the current operation is sketcher sub-operation then we have to create 
//    // Costomized operation
//    if (aOperation->id().toStdString() == SketchPlugin_Sketch::ID()) {
//      PartSet_Operation* aPSOp = new PartSet_Operation(theFeatureId.c_str(), this);
//      aPSOp->setWorkshop(myWorkshop);
//      connect(myWorkshop, SIGNAL(selectionChanged()), aPSOp, SLOT(onSelectionChanged()));
//      return aPSOp;
//    }
//  }
//  return ModuleBase_IModule::getNewOperation(theFeatureId);
//}


void PartSet_Module::onMousePressed(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (!(theEvent->buttons() & Qt::LeftButton))
    return;
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  // Use only for sketch operations
  if (aOperation && myCurrentSketch) {
    if (!PartSet_Tools::sketchPlane(myCurrentSketch))
      return;

    bool isSketcher = (aOperation->id().toStdString() == SketchPlugin_Sketch::ID());
    bool isSketchOpe = sketchOperationIdList().contains(aOperation->id());

    // Avoid non-sketch operations
    if ((!isSketchOpe) && (!isSketcher))
      return;

    bool isEditing = aOperation->isEditOperation();

    // Ignore creation sketch operation
    if ((!isSketcher) && (!isEditing))
      return;

    // Remember highlighted objects for editing
    ModuleBase_ISelection* aSelect = myWorkshop->selection();
    QList<ModuleBase_ViewerPrs> aObjects = aSelect->getHighlighted();
     myEditingFeatures.clear();
    if (aObjects.size() > 0) {
      foreach(ModuleBase_ViewerPrs aPrs, aObjects) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(aObjects.first().object());
        if (aFeature)
          myEditingFeatures.append(aFeature);
      }
    } 
    // If nothing highlighted - return
    if (myEditingFeatures.size() == 0)
      return;

    if (isSketcher) {
      CompositeFeaturePtr aSketch = 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aOperation->feature());
      if (!PartSet_Tools::sketchPlane(aSketch))
        return;
      
      //myCurrentSketch = aOperation->feature();
      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurX, myCurY);
      myDragDone = false;
      myWorkshop->viewer()->enableSelection(false);

      launchEditing();

    } else if (isSketchOpe && isEditing) {
      aOperation->abort();

      //myCurrentSketch = aOperation->parentFeature();
      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurX, myCurY);
      myDragDone = false;
      myWorkshop->viewer()->enableSelection(false);

      QTimer::singleShot(10, this, SLOT(launchEditing()));
    }
  }
}


void PartSet_Module::get2dPoint(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent, 
                                double& theX, double& theY)
{
  Handle(V3d_View) aView = theWnd->v3dView();
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
  PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, theX, theY);
}


void PartSet_Module::launchEditing()
{
  if (myEditingFeatures.size() == 1) {
    FeaturePtr aFeature = myEditingFeatures.first();
    std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
              std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if (aSPFeature) {
      editFeature(aSPFeature);
    }
  }
}

void PartSet_Module::onMouseReleased(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myIsDragging) {
    myIsDragging = false;
    myWorkshop->viewer()->enableSelection(true);
    if (myDragDone)
      myWorkshop->currentOperation()->commit();
  }
}


void PartSet_Module::onMouseMoved(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myIsDragging) {
    Handle(V3d_View) aView = theWnd->v3dView();
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
    double aX, aY;
    PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, aX, aY);
    double dX = myCurX - aX;
    double dY = myCurY - aY;

    ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
    ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
    QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
    foreach(ModuleBase_ModelWidget* aWgt, aWidgets) {
      PartSet_WidgetPoint2D* aWgt2d = dynamic_cast<PartSet_WidgetPoint2D*>(aWgt);
      if (aWgt2d) {
        aWgt2d->setPoint(aWgt2d->x() - dX, aWgt2d->y() - dY);
      }
    }
    myDragDone = true;
    myCurX = aX;
    myCurY = aY;
  }
}


QStringList PartSet_Module::sketchOperationIdList() const
{
  QStringList aIds;
  aIds << SketchPlugin_Line::ID().c_str();
  aIds << SketchPlugin_Point::ID().c_str();
  aIds << SketchPlugin_Arc::ID().c_str();
  aIds << SketchPlugin_Circle::ID().c_str();
  aIds << SketchPlugin_ConstraintLength::ID().c_str();
  aIds << SketchPlugin_ConstraintDistance::ID().c_str();
  aIds << SketchPlugin_ConstraintRigid::ID().c_str();
  aIds << SketchPlugin_ConstraintRadius::ID().c_str();
  aIds << SketchPlugin_ConstraintPerpendicular::ID().c_str();
  aIds << SketchPlugin_ConstraintParallel::ID().c_str();
  return aIds;
}

void PartSet_Module::onVertexSelected(ObjectPtr theObject, const TopoDS_Shape& theShape)
{
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (aOperation->id().toStdString() == SketchPlugin_Line::ID()) {
    /// If last line finished on vertex the lines creation sequence has to be break
    ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPanel->modelWidgets();
    if (aWidgets.last() == aPanel->activeWidget()) {
      breakOperationSequence();
      PartSet_WidgetPoint2D* aPnt2dWgt = dynamic_cast<PartSet_WidgetPoint2D*>(aPanel->activeWidget());
      PartSet_Tools::setConstraints(myCurrentSketch, aOperation->feature(), 
        SketchPlugin_Line::END_ID(), aPnt2dWgt->x(), aPnt2dWgt->y());
    }
  }
}


//void PartSet_Module::featureCreated(QAction* theFeature)
//{
//  connect(theFeature, SIGNAL(triggered(bool)), this, SLOT(onFeatureTriggered()));
//}

//std::string PartSet_Module::featureFile(const std::string& theFeatureId)
//{
//  return myFeaturesInFiles[theFeatureId];
//}

/*
 *
 */
//void PartSet_Module::onFeatureTriggered()
//{
//  QAction* aCmd = dynamic_cast<QAction*>(sender());
//  //Do nothing on uncheck
//  if (aCmd->isCheckable() && !aCmd->isChecked())
//    return;
//  launchOperation(aCmd->data().toString());
//}


//void PartSet_Module::onOperationStarted(ModuleBase_Operation* theOperation)
//{
//  XGUI_Workshop* aXWshp = xWorkshop();
//  XGUI_Displayer* aDisplayer = aXWshp->displayer();
//  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(theOperation);
//  if (aPreviewOp) {
//    XGUI_PropertyPanel* aPropPanel = aXWshp->propertyPanel();
//    connect(aPropPanel, SIGNAL(storedPoint2D(ObjectPtr, const std::string&)), this,
//            SLOT(onStorePoint2D(ObjectPtr, const std::string&)), Qt::UniqueConnection);
//
//    //aDisplayer->deactivateObjectsOutOfContext();
//    PartSet_OperationSketch* aSketchOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
//    if (aSketchOp) {
//      if (aSketchOp->isEditOperation()) {
//        setSketchingMode(getSketchPlane(aSketchOp->feature()));
//      } else {
//        aDisplayer->openLocalContext();
//        aDisplayer->activateObjectsOutOfContext(QIntList());
//        myPlaneFilter = new StdSelect_FaceFilter(StdSelect_Plane);
//        aDisplayer->addSelectionFilter(myPlaneFilter);
//        QIntList aModes = sketchSelectionModes(aPreviewOp->feature());
//        aDisplayer->setSelectionModes(aModes);
//      } 
//    }
//  }
//}

//void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
//{
//  if (!theOperation)
//    return;
//  XGUI_Workshop* aXWshp = xWorkshop();
//  XGUI_Displayer* aDisplayer = aXWshp->displayer();
//  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(theOperation);
//  if (aPreviewOp) {
//    XGUI_PropertyPanel* aPropPanel = aXWshp->propertyPanel();
//
//    PartSet_OperationSketch* aSketchOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
//    if (aSketchOp) {
//      aDisplayer->closeLocalContexts();
//    } else {
//      PartSet_OperationFeatureCreate* aCreationOp = 
//        dynamic_cast<PartSet_OperationFeatureCreate*>(aPreviewOp);
//      if (aCreationOp) {
//        // Activate just created object for selection
//        FeaturePtr aFeature = aCreationOp->feature();
//        QIntList aModes = sketchSelectionModes(aFeature);
//        const std::list<ResultPtr>& aResults = aFeature->results();
//        std::list<ResultPtr>::const_iterator anIt, aLast = aResults.end();
//        for (anIt = aResults.begin(); anIt != aLast; anIt++) {
//          aDisplayer->activate(*anIt, aModes);
//        }
//        aDisplayer->activate(aFeature, aModes);
//        aDisplayer->clearSelected();
//      }
//    }
//  }// else {
//    // Activate results of current feature for selection
//    //FeaturePtr aFeature = theOperation->feature();
//    //XGUI_Displayer* aDisplayer = aXWshp->displayer();
//    //std::list<ResultPtr> aResults = aFeature->results();
//    //std::list<ResultPtr>::const_iterator aIt;
//    //for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
//    //  aDisplayer->activate(*aIt);
//    //}    
//  //}
//}

//void PartSet_Module::onContextMenuCommand(const QString& theId, bool isChecked)
//{
//  QList<ObjectPtr> aFeatures = workshop()->selection()->selectedObjects();
//  if (theId == "EDIT_CMD" && (aFeatures.size() > 0)) {
//    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aFeatures.first());
//    if (aFeature)
//      editFeature(aFeature);
//  }
//}

//void PartSet_Module::onMousePressed(QMouseEvent* theEvent)
//{
//  XGUI_Workshop* aXWshp = xWorkshop();
//  PartSet_OperationSketchBase* aPreviewOp = 
//    dynamic_cast<PartSet_OperationSketchBase*>(workshop()->currentOperation());
//  if (aPreviewOp) {
//    ModuleBase_ISelection* aSelection = workshop()->selection();
//    aPreviewOp->mousePressed(theEvent, myWorkshop->viewer(), aSelection);
//  }
//}

//void PartSet_Module::onMouseReleased(QMouseEvent* theEvent)
//{
//  PartSet_OperationSketchBase* aPreviewOp = 
//    dynamic_cast<PartSet_OperationSketchBase*>(myWorkshop->currentOperation());
//  if (aPreviewOp) {
//    ModuleBase_ISelection* aSelection = workshop()->selection();
//    // Initialise operation with preliminary selection
//    aPreviewOp->mouseReleased(theEvent, myWorkshop->viewer(), aSelection);
//  }
//}

//void PartSet_Module::onMouseMoved(QMouseEvent* theEvent)
//{
//  PartSet_OperationSketchBase* aPreviewOp = 
//    dynamic_cast<PartSet_OperationSketchBase*>(myWorkshop->currentOperation());
//  if (aPreviewOp)
//    aPreviewOp->mouseMoved(theEvent, myWorkshop->viewer());
//}

//void PartSet_Module::onKeyRelease(QKeyEvent* theEvent)
//{
//  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
//  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
//  if (aPreviewOp) {
//    aPreviewOp->keyReleased(theEvent->key());
//  }
//}

//void PartSet_Module::onMouseDoubleClick(QMouseEvent* theEvent)
//{
//  PartSet_OperationSketchBase* aPreviewOp = 
//    dynamic_cast<PartSet_OperationSketchBase*>(myWorkshop->currentOperation());
//  Handle(V3d_View) aView = myWorkshop->viewer()->activeView();
//  if (aPreviewOp && (!aView.IsNull())) {
//    ModuleBase_ISelection* aSelection = workshop()->selection();
//    // Initialise operation with preliminary selection
//    aPreviewOp->mouseDoubleClick(theEvent, aView, aSelection);
//  }
//}

//void PartSet_Module::onPlaneSelected(double theX, double theY, double theZ)
//{
//  myWorkshop->viewer()->setViewProjection(theX, theY, theZ);
//}

//void PartSet_Module::onSketchLaunched()
//{
//  xWorkshop()->actionsMgr()->update();
//  // Set working plane
//  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
//  FeaturePtr aSketch = anOperation->feature();
//  setSketchingMode(getSketchPlane(aSketch));
//}

//void PartSet_Module::onFitAllView()
//{
//  myWorkshop->viewer()->fitAll();
//}

//void PartSet_Module::onRestartOperation(std::string theName, ObjectPtr theObject)
//{
//  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
//
//  std::string aKind = aFeature ? aFeature->getKind() : "";
//  ModuleBase_Operation* anOperation = createOperation(theName, aKind);
//
//  PartSet_OperationSketchBase* aSketchOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
//  if (aSketchOp) {
//    PartSet_OperationFeatureCreate* aCreateOp = dynamic_cast<PartSet_OperationFeatureCreate*>(anOperation);
//    if (aCreateOp)
//      aCreateOp->initFeature(aFeature);
//    else {
//      anOperation->setFeature(aFeature);
//    }
//    ModuleBase_ISelection* aSelection = workshop()->selection();
//    // Initialise operation with preliminary selection
//    aSketchOp->initSelection(aSelection, myWorkshop->viewer());
//  } else if (aFeature) { // In case of edit operation: set the previously created feature to the operation
//    anOperation->setFeature(aFeature);
//    ////Deactivate result of current feature in order to avoid its selection
//    XGUI_Displayer* aDisplayer = xWorkshop()->displayer();
//    std::list<ResultPtr> aResults = aFeature->results();
//    std::list<ResultPtr>::const_iterator aIt;
//    for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
//      aDisplayer->deactivate(*aIt);
//    }
//  }
//  sendOperation(anOperation);
//  xWorkshop()->actionsMgr()->updateCheckState();
//}

//void PartSet_Module::onMultiSelectionEnabled(bool theEnabled)
//{
//  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
//  aViewer->enableMultiselection(theEnabled);
//}

//void PartSet_Module::onStopSelection(const QList<ObjectPtr>& theFeatures, const bool isStop)
//{
//  XGUI_Displayer* aDisplayer = xWorkshop()->displayer();
//  //if (!isStop) {
//  //  foreach(ObjectPtr aObject, theFeatures) {
//  //    activateFeature(aObject);
//  //  }
//  //}
//  aDisplayer->stopSelection(theFeatures, isStop, false);
//
//  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
//  aViewer->enableSelection(!isStop);
//
//  aDisplayer->updateViewer();
//}

//void PartSet_Module::onSetSelection(const QList<ObjectPtr>& theFeatures)
//{
//  XGUI_Displayer* aDisplayer = xWorkshop()->displayer();
//  aDisplayer->setSelected(theFeatures, false);
//  aDisplayer->updateViewer();
//}

//void PartSet_Module::setSketchingMode(const gp_Pln& thePln)
//{
//  XGUI_Displayer* aDisplayer = xWorkshop()->displayer();
//  if (!myPlaneFilter.IsNull()) {
//    aDisplayer->removeSelectionFilter(myPlaneFilter);
//    myPlaneFilter.Nullify();
//  }
//  QIntList aModes;
//  // Clear standard selection modes
//  aDisplayer->setSelectionModes(aModes);
//  aDisplayer->openLocalContext();
//
//  // Set filter
//  mySketchFilter = new ModuleBase_ShapeInPlaneFilter(thePln);
//  aDisplayer->addSelectionFilter(mySketchFilter);
//
//  // Get default selection modes
//  aModes = sketchSelectionModes(ObjectPtr());
//  aDisplayer->activateObjectsOutOfContext(aModes);
//}

//void PartSet_Module::onFeatureConstructed(ObjectPtr theFeature, int theMode)
//{
//  bool isDisplay = theMode != PartSet_OperationSketchBase::FM_Hide;
//  ModuleBase_Operation* aCurOperation = myWorkshop->currentOperation();
//  PartSet_OperationSketchBase* aPrevOp = dynamic_cast<PartSet_OperationSketchBase*>(aCurOperation);
//  if (aPrevOp) {
//    std::list<FeaturePtr> aList = aPrevOp->subFeatures();
//    XGUI_Displayer* aDisplayer = xWorkshop()->displayer();
//    QIntList aModes = sketchSelectionModes(aPrevOp->feature());
//    std::list<FeaturePtr>::iterator aSFIt;
//    for (aSFIt = aList.begin(); aSFIt != aList.end(); ++aSFIt) {
//      std::list<ResultPtr> aResults = (*aSFIt)->results();
//      std::list<ResultPtr>::iterator aIt;
//      for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
//        if (!isDisplay)
//          aDisplayer->erase((*aIt), false);
//      }
//      if (!isDisplay)
//        aDisplayer->erase((*aSFIt), false);
//    }
//    //aDisplayer->deactivateObjectsOutOfContext();
//  }
//  if (isDisplay)
//    ModelAPI_EventCreator::get()->sendUpdated(
//        theFeature, Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
//}

//ModuleBase_Operation* PartSet_Module::createOperation(const std::string& theCmdId,
//                                                      const std::string& theFeatureKind)
//{
//  // create the operation
//  ModuleBase_Operation* anOperation = 0;
//  if (theCmdId == PartSet_OperationSketch::Type()) {
//    anOperation = new PartSet_OperationSketch(theCmdId.c_str(), this);
//  } else {
//    ModuleBase_Operation* aCurOperation = myWorkshop->currentOperation();
//    CompositeFeaturePtr aSketch;
//    PartSet_OperationSketchBase* aPrevOp = dynamic_cast<PartSet_OperationSketchBase*>(aCurOperation);
//    if (aPrevOp) {
//      aSketch = aPrevOp->sketch();
//    }
//    if (PartSet_OperationFeatureCreate::canProcessKind(theCmdId)) {
//      anOperation = new PartSet_OperationFeatureCreate(theCmdId.c_str(), this, aSketch);
//    } else if (theCmdId == PartSet_OperationFeatureEdit::Type()) {
//      anOperation = new PartSet_OperationFeatureEdit(theCmdId.c_str(), this, aSketch);
//    }
//  }
//
//  if (!anOperation) {
//    anOperation = new ModuleBase_Operation(theCmdId.c_str(), this);
//  }
//
//  // set operation description and list of widgets corresponding to the feature xml definition
//  std::string aFeatureKind = theFeatureKind.empty() ? theCmdId : theFeatureKind;
//
//  std::string aPluginFileName = featureFile(aFeatureKind);
//  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginFileName);
//  aWdgReader.readAll();
//  std::string aXmlCfg = aWdgReader.featureWidgetCfg(aFeatureKind);
//  std::string aDescription = aWdgReader.featureDescription(aFeatureKind);
//
//  anOperation->getDescription()->setDescription(QString::fromStdString(aDescription));
//  anOperation->getDescription()->setXmlRepresentation(QString::fromStdString(aXmlCfg));
//
//  // connect the operation
//  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
//  if (aPreviewOp) {
//    connect(aPreviewOp, SIGNAL(featureConstructed(ObjectPtr, int)), this,
//            SLOT(onFeatureConstructed(ObjectPtr, int)));
//    connect(aPreviewOp, SIGNAL(restartRequired(std::string, ObjectPtr)), this,
//            SLOT(onRestartOperation(std::string, ObjectPtr)));
//    // If manage multi selection the it will be impossible to select more then one
//    // object under operation Edit
////    connect(aPreviewOp, SIGNAL(multiSelectionEnabled(bool)), this,
////            SLOT(onMultiSelectionEnabled(bool)));
//
//    connect(aPreviewOp, SIGNAL(stopSelection(const QList<ObjectPtr>&, const bool)), this,
//            SLOT(onStopSelection(const QList<ObjectPtr>&, const bool)));
//    connect(aPreviewOp, SIGNAL(setSelection(const QList<ObjectPtr>&)), this,
//            SLOT(onSetSelection(const QList<ObjectPtr>&)));
//
//    PartSet_OperationSketch* aSketchOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
//    if (aSketchOp) {
//      connect(aSketchOp, SIGNAL(planeSelected(double, double, double)), this,
//              SLOT(onPlaneSelected(double, double, double)));
//      connect(aSketchOp, SIGNAL(fitAllView()), this, SLOT(onFitAllView()));
//      connect(aSketchOp, SIGNAL(launchSketch()), this, SLOT(onSketchLaunched()));
//    }
//  }
//
//  return anOperation;
//}


//void PartSet_Module::updateCurrentPreview(const std::string& theCmdId)
//{
//  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
//  if (!anOperation)
//    return;
//
//  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
//  if (!aPreviewOp)
//    return;
//
//  FeaturePtr aFeature = aPreviewOp->feature();
//  if (!aFeature || aFeature->getKind() != theCmdId)
//    return;
//
//  XGUI_Displayer* aDisplayer = xWorkshop()->displayer();
//  // Hide result of sketch
//  std::list<ResultPtr> aResults = aFeature->results();
//  std::list<ResultPtr>::const_iterator aIt;
//  for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt)
//    aDisplayer->erase(*aIt, false);
//
//  std::list<FeaturePtr> aList = aPreviewOp->subFeatures();
//
//  std::list<FeaturePtr>::const_iterator anIt = aList.begin(), aLast = aList.end();
//  for (; anIt != aLast; anIt++) {
//    std::shared_ptr<SketchPlugin_Feature> aSPFeature = std::dynamic_pointer_cast<
//        SketchPlugin_Feature>(*anIt);
//    if (!aSPFeature)
//      continue;
//    std::list<ResultPtr> aResults = aSPFeature->results();
//    std::list<ResultPtr>::const_iterator aRIt;
//    for (aRIt = aResults.cbegin(); aRIt != aResults.cend(); ++aRIt) {
//      aDisplayer->display((*aRIt), false);
//      aDisplayer->activate((*aRIt), sketchSelectionModes((*aRIt)));
//    }
//    aDisplayer->display(aSPFeature, false);
//    aDisplayer->activate(aSPFeature, sketchSelectionModes(aSPFeature));
//  }
//  aDisplayer->updateViewer();
//}

//void PartSet_Module::editFeature(FeaturePtr theFeature)
//{
//  if (!theFeature)
//    return;

//  if (theFeature->getKind() == SKETCH_KIND) {
  //FeaturePtr aFeature = theFeature;
  //if (XGUI_Tools::isModelObject(aFeature)) {
  //  ObjectPtr aObject = std::dynamic_pointer_cast<ModelAPI_Object>(aFeature);
  //  aFeature = aObject->featureRef();
  //}

  //if (aFeature) {
//  onRestartOperation(theFeature->getKind(), theFeature);
//  updateCurrentPreview(theFeature->getKind());
  //}
//  }
//}

//void PartSet_Module::onStorePoint2D(ObjectPtr theFeature, const std::string& theAttribute)
//{
//  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theFeature);
//
//  PartSet_OperationSketchBase* aPreviewOp = 
//    dynamic_cast<PartSet_OperationSketchBase*>(myWorkshop->currentOperation());
//  if (!aPreviewOp)
//    return;
//
//  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
//      aFeature->data()->attribute(theAttribute));
//
//  PartSet_Tools::setConstraints(aPreviewOp->sketch(), aFeature, theAttribute, aPoint->x(),
//                                aPoint->y());
//}

QWidget* PartSet_Module::createWidgetByType(const std::string& theType, QWidget* theParent,
                                            Config_WidgetAPI* theWidgetApi, std::string theParentId,
                                            QList<ModuleBase_ModelWidget*>& theModelWidgets)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  if (theType == "sketch-start-label") {
    PartSet_WidgetSketchLabel* aWgt = new PartSet_WidgetSketchLabel(theParent, theWidgetApi, theParentId);
    aWgt->setWorkshop(aWorkshop);
    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  } else if (theType == "sketch-2dpoint_selector") {
    PartSet_WidgetPoint2D* aWgt = new PartSet_WidgetPoint2D(theParent, theWidgetApi, theParentId);
    aWgt->setWorkshop(aWorkshop);
    aWgt->setSketch(myCurrentSketch);

    connect(aWgt, SIGNAL(vertexSelected(ObjectPtr, const TopoDS_Shape&)), 
      this, SLOT(onVertexSelected(ObjectPtr, const TopoDS_Shape&)));

    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  } if (theType == "point2ddistance") {
    PartSet_WidgetPoint2dDistance* aWgt = new PartSet_WidgetPoint2dDistance(theParent, theWidgetApi, theParentId);
    aWgt->setWorkshop(aWorkshop);
    aWgt->setSketch(myCurrentSketch);

    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  } if (theType == "sketch_shape_selector") {
    PartSet_WidgetShapeSelector* aWgt = 
      new PartSet_WidgetShapeSelector(theParent, workshop(), theWidgetApi, theParentId);
    aWgt->setSketcher(myCurrentSketch);

    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  }else
    return 0;
}


//XGUI_Workshop* PartSet_Module::xWorkshop() const
//{
//  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
//  if (aConnector) {
//    return aConnector->workshop();
//  }
//  return 0;
//}


//QIntList PartSet_Module::sketchSelectionModes(ObjectPtr theFeature)
//{
//  QIntList aModes;
//  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theFeature);
//  if (aFeature) {
//    if (aFeature->getKind() == SketchPlugin_Sketch::ID()) {
//      aModes.append(TopAbs_FACE);
//      return aModes;
//    } else if (PartSet_Tools::isConstraintFeature(aFeature->getKind())) {
//      aModes.append(AIS_DSM_Text);
//      aModes.append(AIS_DSM_Line);
//      return aModes;
//    }
//  } 
//  aModes.append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum) TopAbs_VERTEX));
//  aModes.append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum) TopAbs_EDGE));
//  return aModes;
//}


//gp_Pln PartSet_Module::getSketchPlane(FeaturePtr theSketch) const
//{
//  DataPtr aData = theSketch->data();
//  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
//      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
//  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
//      aData->attribute(SketchPlugin_Sketch::NORM_ID()));
//  gp_Pnt aOrig(anOrigin->x(), anOrigin->y(), anOrigin->z());
//  gp_Dir aDir(aNorm->x(), aNorm->y(), aNorm->z());
//  return gp_Pln(aOrig, aDir);
//}


//void PartSet_Module::onSelectionChanged()
//{
//  ModuleBase_ISelection* aSelect = myWorkshop->selection();
//  QList<ModuleBase_ViewerPrs> aSelected = aSelect->getSelected();
//  // We need to stop edit operation if selection is cleared
//  if (aSelected.size() == 0) {
//    // do not perform commit of the current edit operation here, because
//    // this functionality is realized inside this operation
//    /*PartSet_OperationFeatureEdit* anEditOp = 
//      dynamic_cast<PartSet_OperationFeatureEdit*>(myWorkshop->currentOperation());
//    if (!anEditOp)
//      return;
//    anEditOp->commit();*/
//  } else {
//    PartSet_OperationSketchBase* aSketchOp = 
//      dynamic_cast<PartSet_OperationSketchBase*>(myWorkshop->currentOperation());
//    if (aSketchOp) {
//      aSketchOp->selectionChanged(aSelect);
//    }
//  }
//}
