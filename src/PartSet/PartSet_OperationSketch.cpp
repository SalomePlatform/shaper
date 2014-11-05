// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketch.h>

#include <PartSet_OperationFeatureEdit.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintLength.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Events.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAPI_XYZ.h>

#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IViewer.h>
#include <Events_Loop.h>

#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_DimensionOwner.hxx>
#include <AIS_LengthDimension.hxx>
#include <V3d_View.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QMouseEvent>

using namespace std;

PartSet_OperationSketch::PartSet_OperationSketch(const QString& theId, QObject* theParent)
    : PartSet_OperationSketchBase(theId, theParent)
{
}

PartSet_OperationSketch::~PartSet_OperationSketch()
{
}

CompositeFeaturePtr PartSet_OperationSketch::sketch() const
{
  return boost::dynamic_pointer_cast<ModelAPI_CompositeFeature>(feature());
}

void PartSet_OperationSketch::mousePressed(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer, ModuleBase_ISelection* theSelection)
{
  if (hasSketchPlane()) {
    // if shift button is pressed and there are some already selected objects, the operation should
    // not be started. We just want to combine some selected objects.
    bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
    QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();
    QList<ModuleBase_ViewerPrs> aHighlighted = theSelection->getHighlighted();
    //if (aHasShift && (aSelected.size() > 0)) {
      foreach(ModuleBase_ViewerPrs aPrs, aHighlighted)
        aSelected.append(aPrs);
    //}
    //if (aHasShift && aSelected.size() > 0)
    //  return;

    // there should be a start of operation, which uses the pre-highlighted objects,
    // the selected ones are collected here and are processed by a mouse move
    if (aHighlighted.size() == 1) {
    //if (aSelected.size() > 0) {
      ObjectPtr aFeature = aSelected.first().object();
      if (aFeature) {
        std::string anOperationType = PartSet_OperationFeatureEdit::Type();
        restartOperation(anOperationType, aFeature);
      }
    }
    else
      myFeatures = aHighlighted;
    //else
    //myFeatures = aSelected;

  } 
}


void PartSet_OperationSketch::selectionChanged(ModuleBase_ISelection* theSelection)
{
  if (hasSketchPlane()) 
    return;

  QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();
  if (!aSelected.empty()) {
    ModuleBase_ViewerPrs aPrs = aSelected.first();
    // We have to select a plane before any operation
    TopoDS_Shape aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      setSketchPlane(aShape);
      // If selection is not a sketcher presentation then it has to be stored as 
      // External shape
      if (feature() != aPrs.object()) {
        boost::shared_ptr<SketchPlugin_Sketch> aSketch = 
          boost::dynamic_pointer_cast<SketchPlugin_Sketch>(feature());
        DataPtr aData = aSketch->data();
        AttributeSelectionPtr aSelAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeSelection>
          (aData->attribute(SketchPlugin_Feature::EXTERNAL_ID()));
        if (aSelAttr) {
          ResultPtr aRes = boost::dynamic_pointer_cast<ModelAPI_Result>(aPrs.object());
          if (aRes) {
            GeomShapePtr aShapePtr(new GeomAPI_Shape());
            aShapePtr->setImpl(new TopoDS_Shape(aShape));
            aSelAttr->setValue(aRes, aShapePtr);
          }
        }
      }
    }
  }
}


void PartSet_OperationSketch::mouseReleased(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer,
                                            ModuleBase_ISelection* theSelection)
{
  QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();
  if (hasSketchPlane()) {
    /// TODO: OCC bug: 25034 - the highlighted list should be filled not only for AIS_Shape
    /// but for other IO, for example constraint dimensions.
    /// It is empty and we have to use the process mouse release to start edition operation
    /// for these objects
    if (aSelected.size() == 1) {
      ObjectPtr aObject = aSelected.first().object();
      if (aObject) {
        restartOperation(PartSet_OperationFeatureEdit::Type(), aObject);
      }
    }
  }
}

void PartSet_OperationSketch::mouseMoved(QMouseEvent* theEvent, ModuleBase_IViewer* theViewer)
{
  if (!hasSketchPlane() || !(theEvent->buttons() & Qt::LeftButton) || myFeatures.empty())
    return;

  if (myFeatures.size() != 1) {
    Handle(V3d_View) aView = theViewer->activeView();
    FeaturePtr aFeature = PartSet_Tools::nearestFeature(theEvent->pos(), aView, feature(),
                                                        myFeatures);
    if (aFeature)
      restartOperation(PartSet_OperationFeatureEdit::Type(), aFeature);
  }
}

std::list<FeaturePtr> PartSet_OperationSketch::subFeatures() const
{
  std::list<FeaturePtr> aFeaList;
  FeaturePtr aFeature = feature();
  if (!aFeature)
    return aFeaList;

  boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();
  if (!aData->isValid())
    return std::list<FeaturePtr>();
  boost::shared_ptr<ModelAPI_AttributeRefList> aRefList = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(aData->attribute(SketchPlugin_Sketch::FEATURES_ID()));

  std::list<ObjectPtr> aList = aRefList->list();
  std::list<ObjectPtr>::iterator aIt;
  for (aIt = aList.begin(); aIt != aList.end(); ++aIt) {
    FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(*aIt);
    if (aFeature)
      aFeaList.push_back(aFeature);
  }
  return aFeaList;
}

void PartSet_OperationSketch::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();
  emit featureConstructed(feature(), FM_Hide);
}

void PartSet_OperationSketch::afterCommitOperation()
{
  FeaturePtr aFeature = feature();
  std::list<ResultPtr> aResults = aFeature->results();
  std::list<ResultPtr>::const_iterator aIt;
  Events_ID anEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TOSHOW);
  for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
    ModelAPI_EventCreator::get()->sendUpdated(*aIt, anEvent);
  }
  Events_Loop::loop()->flush(anEvent);
}

bool PartSet_OperationSketch::isNestedOperationsEnabled() const
{
  return hasSketchPlane();
}

void PartSet_OperationSketch::startOperation()
{
  PartSet_OperationSketchBase::startOperation();
  if (!isEditOperation())
    emit fitAllView();
}

bool PartSet_OperationSketch::hasSketchPlane() const
{
  bool aHasPlane = false;

  if (feature()) {
    boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
    AttributeDoublePtr anAttr;
    boost::shared_ptr<GeomDataAPI_Dir> aNormal = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    aHasPlane = aNormal && !(aNormal->x() == 0 && aNormal->y() == 0 && aNormal->z() == 0);
  }
  return aHasPlane;
}

void PartSet_OperationSketch::setSketchPlane(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
    return;

  // get selected shape
  boost::shared_ptr<GeomAPI_Shape> aGShape(new GeomAPI_Shape);
  aGShape->setImpl(new TopoDS_Shape(theShape));

  // get plane parameters
  boost::shared_ptr<GeomAPI_Pln> aPlane = GeomAlgoAPI_FaceBuilder::plane(aGShape);

  // set plane parameters to feature
  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  double anA, aB, aC, aD;
  aPlane->coefficients(anA, aB, aC, aD);

  // calculate attributes of the sketch
  boost::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
  boost::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
  boost::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
  aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
  boost::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
  // X axis is preferable to be dirX on the sketch
  const double tol = Precision::Confusion();
  bool isX = fabs(anA - 1.0) < tol && fabs(aB) < tol && fabs(aC) < tol;
  boost::shared_ptr<GeomAPI_Dir> aTempDir(
      isX ? new GeomAPI_Dir(0, 1, 0) : new GeomAPI_Dir(1, 0, 0));
  boost::shared_ptr<GeomAPI_Dir> aYDir(new GeomAPI_Dir(aNormDir->cross(aTempDir)));
  boost::shared_ptr<GeomAPI_Dir> aXDir(new GeomAPI_Dir(aYDir->cross(aNormDir)));

  boost::shared_ptr<GeomDataAPI_Point> anOrigin = boost::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  anOrigin->setValue(anOrigPnt);
  boost::shared_ptr<GeomDataAPI_Dir> aNormal = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::NORM_ID()));
  aNormal->setValue(aNormDir);
  boost::shared_ptr<GeomDataAPI_Dir> aDirX = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  aDirX->setValue(aXDir);
  boost::shared_ptr<GeomDataAPI_Dir> aDirY = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRY_ID()));
  aDirY->setValue(aYDir);
  boost::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();

  flushUpdated();

  emit featureConstructed(feature(), FM_Hide);
  emit planeSelected(aDir->x(), aDir->y(), aDir->z());
}


bool PartSet_OperationSketch::isGranted(ModuleBase_Operation* theOperation) const
{
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(theOperation);
  return aPreviewOp != NULL;
}

