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

#include <Events_Loop.h>

#include <StdSelect_TypeOfFace.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>

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


/// Returns list of unique objects by sum of objects from List1 and List2
QList<ObjectPtr> getSumList(const QList<ModuleBase_ViewerPrs>& theList1,
                                       const QList<ModuleBase_ViewerPrs>& theList2)
{
  QList<ObjectPtr> aRes;
  foreach (ModuleBase_ViewerPrs aPrs, theList1) {
    if (!aRes.contains(aPrs.object()))
      aRes.append(aPrs.object());
  }
  foreach (ModuleBase_ViewerPrs aPrs, theList2) {
    if (!aRes.contains(aPrs.object()))
      aRes.append(aPrs.object());
  }
  return aRes;
}

/*!Create and return new instance of XGUI_Module*/
extern "C" PARTSET_EXPORT ModuleBase_IModule* createModule(ModuleBase_IWorkshop* theWshop)
{
  return new PartSet_Module(theWshop);
}

PartSet_Module::PartSet_Module(ModuleBase_IWorkshop* theWshop)
  : ModuleBase_IModule(theWshop), 
  myIsDragging(false), myRestartingMode(RM_LastFeatureUse), myDragDone(false)
{
  //myWorkshop = dynamic_cast<XGUI_Workshop*>(theWshop);
  ModuleBase_IViewer* aViewer = aViewer = theWshop->viewer();
  connect(aViewer, SIGNAL(mousePress(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMousePressed(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseReleased(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseMoved(ModuleBase_IViewWindow*, QMouseEvent*)));

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();

  XGUI_OperationMgr* anOpMgr = aWorkshop->operationMgr();
  connect(anOpMgr, SIGNAL(keyEnterReleased()), this, SLOT(onEnterReleased()));

  connect(aViewer, SIGNAL(keyRelease(ModuleBase_IViewWindow*, QKeyEvent*)),
          this, SLOT(onKeyRelease(ModuleBase_IViewWindow*, QKeyEvent*)));
}

PartSet_Module::~PartSet_Module()
{
  if (!myDocumentShapeFilter.IsNull())
    myDocumentShapeFilter.Nullify();
  if (!myPlaneFilter.IsNull())
    myPlaneFilter.Nullify();
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
  if (aSPFeature && (myRestartingMode != RM_None)) {
    myLastOperationId = theOperation->id();
    myLastFeature = myRestartingMode == RM_LastFeatureUse ? theOperation->feature() : FeaturePtr();
    launchOperation(myLastOperationId);
  } else {
    breakOperationSequence();
  }
}

void PartSet_Module::breakOperationSequence()
{
  myLastOperationId = "";
  myLastFeature = FeaturePtr();
  myRestartingMode = RM_None;

}

void PartSet_Module::onOperationAborted(ModuleBase_Operation* theOperation)
{
  breakOperationSequence();
}

void PartSet_Module::onOperationStarted(ModuleBase_Operation* theOperation)
{
  myRestartingMode = RM_LastFeatureUse;
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


    if (myPlaneFilter.IsNull()) 
      myPlaneFilter = new ModuleBase_ShapeInPlaneFilter();
    myWorkshop->viewer()->addSelectionFilter(myPlaneFilter);
    if (theOperation->isEditOperation()) {
      // If it is editing of sketch then it means that plane is already defined
      std::shared_ptr<GeomAPI_Pln> aPln = PartSet_Tools::sketchPlane(myCurrentSketch);
      myPlaneFilter->setPlane(aPln->impl<gp_Pln>());
    }
  }
  if (myDocumentShapeFilter.IsNull())
    myDocumentShapeFilter = new ModuleBase_ShapeDocumentFilter(myWorkshop);
  myWorkshop->viewer()->addSelectionFilter(myDocumentShapeFilter);
}

void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
{
  if (theOperation->id().toStdString() == SketchPlugin_Sketch::ID()) {
    DataPtr aData = myCurrentSketch->data();
    if ((!aData) || (!aData->isValid())) {
      // The sketch was aborted
      myCurrentSketch = CompositeFeaturePtr();
      return; 
    }
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
    myWorkshop->viewer()->removeSelectionFilter(myPlaneFilter);
  }
  myWorkshop->viewer()->removeSelectionFilter(myDocumentShapeFilter);
}

void PartSet_Module::onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>& thePln)
{
  myPlaneFilter->setPlane(thePln->impl<gp_Pln>());
}


void PartSet_Module::propertyPanelDefined(ModuleBase_Operation* theOperation)
{
  ModuleBase_IPropertyPanel* aPanel = theOperation->propertyPanel();
  // Restart last operation type 
  if ((theOperation->id() == myLastOperationId) && myLastFeature) {
    ModuleBase_ModelWidget* aWgt = aPanel->activeWidget();
    if (theOperation->id().toStdString() == SketchPlugin_Line::ID()) {
      // Initialise new line with first point equal to end of previous
      PartSet_WidgetPoint2D* aPnt2dWgt = dynamic_cast<PartSet_WidgetPoint2D*>(aWgt);
      if (aPnt2dWgt) {
        std::shared_ptr<ModelAPI_Data> aData = myLastFeature->data();
        std::shared_ptr<GeomDataAPI_Point2D> aPoint = 
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::END_ID()));
        if (aPoint) {
          aPnt2dWgt->setPoint(aPoint->x(), aPoint->y());
          PartSet_Tools::setConstraints(myCurrentSketch, theOperation->feature(), 
            aWgt->attributeID(), aPoint->x(), aPoint->y());
          theOperation->propertyPanel()->activateNextWidget(aPnt2dWgt);
        }
      }
    }
  } else {
    // Start editing constraint
    if (theOperation->isEditOperation()) {
      std::string aId = theOperation->id().toStdString();
      if (sketchOperationIdList().contains(QString(aId.c_str()))) {
        if ((aId == SketchPlugin_ConstraintRadius::ID()) ||
            (aId == SketchPlugin_ConstraintLength::ID()) || 
            (aId == SketchPlugin_ConstraintDistance::ID())) {
          // Find and activate widget for management of point for dimension line position
          QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
          foreach (ModuleBase_ModelWidget* aWgt, aWidgets) {
            PartSet_WidgetPoint2D* aPntWgt = dynamic_cast<PartSet_WidgetPoint2D*>(aWgt);
            if (aPntWgt) {
              aPanel->activateWidget(aPntWgt);
              return;
            }
          }
        } 
      }
    }
  }
}


void PartSet_Module::onSelectionChanged()
{
  // Editing of constraints can be done on selection
  ModuleBase_ISelection* aSelect = myWorkshop->selection();
  QList<ModuleBase_ViewerPrs> aSelected = aSelect->getSelected();
  if (aSelected.size() == 1) {
    ModuleBase_ViewerPrs aPrs = aSelected.first();
    ObjectPtr aObject = aPrs.object();
    FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
    if (aFeature) {
      std::string aId = aFeature->getKind();
      if ((aId == SketchPlugin_ConstraintRadius::ID()) ||
          (aId == SketchPlugin_ConstraintLength::ID()) || 
          (aId == SketchPlugin_ConstraintDistance::ID())) {
        editFeature(aFeature);
      }
    }
  }
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

    if (theEvent->modifiers()) {
      // If user performs multiselection
      if (isSketchOpe && (!isSketcher))
        if (!aOperation->commit())
          aOperation->abort();
      return;
    }
    // Remember highlighted objects for editing
    ModuleBase_ISelection* aSelect = myWorkshop->selection();
    QList<ModuleBase_ViewerPrs> aHighlighted = aSelect->getHighlighted();
    QList<ModuleBase_ViewerPrs> aSelected = aSelect->getSelected();
    myEditingFeatures.clear();
    myEditingAttr.clear();
    if ((aHighlighted.size() == 0) && (aSelected.size() == 0)) {
      if (isSketchOpe && (!isSketcher))
        // commit previous operation
        if (!aOperation->commit())
          aOperation->abort();
      return;
    }

    QObjectPtrList aSelObjects = getSumList(aHighlighted, aSelected);
    if ((aHighlighted.size() == 1) && (aSelected.size() == 0)) {
      // Move by selected shape (vertex). Can be used only for single selection
      foreach(ModuleBase_ViewerPrs aPrs, aHighlighted) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(aHighlighted.first().object());
        if (aFeature) {
          myEditingFeatures.append(aFeature);
          TopoDS_Shape aShape = aPrs.shape();
          if (!aShape.IsNull()) {
            if (aShape.ShapeType() == TopAbs_VERTEX) {
              AttributePtr aAttr = PartSet_Tools::findAttributeBy2dPoint(myEditingFeatures.first(), 
                                                                         aShape, myCurrentSketch);
              if (aAttr)
                myEditingAttr.append(aAttr);
            }
          }
        }
      }
    } else {
      // Provide multi-selection. Can be used only for features
      foreach (ObjectPtr aObj, aSelObjects) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
        if (aFeature && (!myEditingFeatures.contains(aFeature)))
          myEditingFeatures.append(aFeature);
      }

    }
    // If nothing highlighted - return
    if (myEditingFeatures.size() == 0)
      return;

    if (isSketcher) {
      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurX, myCurY);
      myDragDone = false;
      myWorkshop->viewer()->enableSelection(false);
      launchEditing();

    } else if (isSketchOpe && isEditing) {
      // If selected another object
      aOperation->abort();

      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurX, myCurY);
      myDragDone = false;
      myWorkshop->viewer()->enableSelection(false);

      // This is necessary in order to finalize previous operation
      QApplication::processEvents();
      launchEditing();
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
  if (myEditingFeatures.size() > 0) {
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
  myWorkshop->viewer()->enableSelection(true);
  if (myIsDragging) {
    myIsDragging = false;
    if (myDragDone) {
      myWorkshop->currentOperation()->commit();
      myEditingFeatures.clear();
      myEditingAttr.clear();
    }
  }
}


void PartSet_Module::onMouseMoved(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myIsDragging) {
    ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
    if (aOperation->id().toStdString() == SketchPlugin_Sketch::ID())
      return; // No edit operation activated

    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);
    Handle(V3d_View) aView = theWnd->v3dView();
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
    double aX, aY;
    PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, aX, aY);
    double dX =  aX - myCurX;
    double dY =  aY - myCurY;

    if ((aOperation->id().toStdString() == SketchPlugin_Line::ID()) &&
        (myEditingAttr.size() == 1) && 
        myEditingAttr.first()) {
      // probably we have prehighlighted point
      AttributePtr aAttr = myEditingAttr.first();
      std::string aAttrId = aAttr->id();
      ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
      QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
      // Find corresponded widget to provide dragging
      foreach (ModuleBase_ModelWidget* aWgt, aWidgets) {
        if (aWgt->attributeID() == aAttrId) {
          PartSet_WidgetPoint2D* aWgt2d = dynamic_cast<PartSet_WidgetPoint2D*>(aWgt);
          if (aWgt2d) {
            aWgt2d->setPoint(aWgt2d->x() + dX, aWgt2d->y() + dY);
            break;
          }
        }
      }
    } else {
      foreach(FeaturePtr aFeature, myEditingFeatures) {
        std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature) { 
          aSketchFeature->move(dX, dY);
          ModelAPI_EventCreator::get()->sendUpdated(aSketchFeature, anEvent);
        }
      }
      Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_MOVED));
      Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
    }
    myDragDone = true;
    myCurX = aX;
    myCurY = aY;
  }
}

void PartSet_Module::onKeyRelease(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
  anOpMgr->onKeyReleased(theEvent);
}

void PartSet_Module::onEnterReleased()
{
  myRestartingMode = RM_LastFeatureEmpty;
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
    connect(aWgt, SIGNAL(planeSelected(const std::shared_ptr<GeomAPI_Pln>&)), 
      this, SLOT(onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>&)));
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

  } if (theType == "sketch_constraint_shape_selector") {
    PartSet_WidgetConstraintShapeSelector* aWgt = 
      new PartSet_WidgetConstraintShapeSelector(theParent, workshop(), theWidgetApi, theParentId);
    aWgt->setSketcher(myCurrentSketch);

    theModelWidgets.append(aWgt);
    return aWgt->getControl();

  }else
    return 0;
}
