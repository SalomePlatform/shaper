#include "PartSet_Module.h"
#include <PartSet_OperationSketch.h>
#include <PartSet_WidgetSketchLabel.h>
#include <PartSet_Validators.h>
#include <PartSet_Tools.h>
#include <PartSet_WidgetPoint2D.h>
#include <PartSet_WidgetPoint2dDistance.h>
#include <PartSet_WidgetShapeSelector.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IPropertyPanel.h>

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

#include <StdSelect_TypeOfFace.hxx>

#include <QObject>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <QApplication>

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
}

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

      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurX, myCurY);
      myDragDone = false;
      myWorkshop->viewer()->enableSelection(false);

      // This is necessary in order to finalize previous operation
      QApplication::processEvents();
      launchEditing();
      //QTimer::singleShot(10, this, SLOT(launchEditing()));
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

/// Returns new instance of operation object (used in createOperation for customization)
ModuleBase_Operation* PartSet_Module::getNewOperation(const std::string& theFeatureId)
{
  if (theFeatureId == PartSet_OperationSketch::Type()) {
    return new PartSet_OperationSketch(theFeatureId.c_str(), this);
  }
  return ModuleBase_IModule::getNewOperation(theFeatureId);
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
