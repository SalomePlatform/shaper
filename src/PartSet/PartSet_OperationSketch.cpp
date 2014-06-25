// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketch.h>

#include <PartSet_OperationFeatureEdit.h>
#include <PartSet_OperationFeatureEditMulti.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintLength.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAPI_XYZ.h>

#include <XGUI_ViewerPrs.h>

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

PartSet_OperationSketch::PartSet_OperationSketch(const QString& theId,
	                                             QObject* theParent)
: PartSet_OperationSketchBase(theId, theParent)
{
}

PartSet_OperationSketch::~PartSet_OperationSketch()
{
}

std::list<int> PartSet_OperationSketch::getSelectionModes(FeaturePtr theFeature) const
{
  std::list<int> aModes;
  if (!hasSketchPlane())
    aModes.push_back(TopAbs_FACE);
  else
    aModes = PartSet_OperationSketchBase::getSelectionModes(theFeature);

  return aModes;
}

void PartSet_OperationSketch::init(FeaturePtr theFeature,
                                   const std::list<XGUI_ViewerPrs>& /*theSelected*/,
                                   const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  if (theFeature)
    setEditingFeature(theFeature);
}

FeaturePtr PartSet_OperationSketch::sketch() const
{
  return feature();
}

void PartSet_OperationSketch::mousePressed(QMouseEvent* theEvent, Handle_V3d_View theView,
                                           const std::list<XGUI_ViewerPrs>& theSelected,
                                           const std::list<XGUI_ViewerPrs>& theHighlighted)
{
  if (!hasSketchPlane()) {
    if (!theHighlighted.empty()) {
      XGUI_ViewerPrs aPrs = theHighlighted.front();
      const TopoDS_Shape& aShape = aPrs.shape();
      if (!aShape.IsNull())
        setSketchPlane(aShape);
    }
  }
  else {
    // if shift button is pressed and there are some already selected objects, the operation should
    // not be started. We just want to combine some selected objects.
    bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
    if (aHasShift && theSelected.size() > 0)
      return;

    if (theHighlighted.size() == 1) {
      FeaturePtr aFeature = theHighlighted.front().feature();
      if (aFeature) {
        std::string anOperationType = PartSet_OperationFeatureEdit::Type();
        if (theSelected.size() > 1)
          anOperationType = PartSet_OperationFeatureEditMulti::Type();
        restartOperation(anOperationType, aFeature);
      }
    }
    else
      myFeatures = theHighlighted;

  }
}

void PartSet_OperationSketch::mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                                            const std::list<XGUI_ViewerPrs>& theSelected,
                                            const std::list<XGUI_ViewerPrs>& theHighlighted)
{
  if (!hasSketchPlane()) {
  }
  else {
    /// TODO: OCC bug: 25034 - the highlighted list should be filled not only for AIS_Shape
    /// but for other IO, for example constraint dimensions.
    /// It is empty and we have to use the process mouse release to start edition operation
    /// for these objects
    if (theSelected.size() == 1) {
      FeaturePtr aFeature = theSelected.front().feature();
      if (aFeature)
        restartOperation(PartSet_OperationFeatureEdit::Type(), aFeature);
    }
  }
}

void PartSet_OperationSketch::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
  if (!hasSketchPlane() || !(theEvent->buttons() &  Qt::LeftButton) || myFeatures.empty())
    return;

  if (myFeatures.size() != 1) {
    FeaturePtr aFeature = PartSet_Tools::nearestFeature(theEvent->pos(), theView, feature(),
                                                        myFeatures);
    if (aFeature)
		restartOperation(PartSet_OperationFeatureEditMulti::Type(), aFeature);
  }
}

std::map<FeaturePtr, boost::shared_ptr<GeomAPI_Shape> >
                                                        PartSet_OperationSketch::subPreview() const
{
  std::map<FeaturePtr, boost::shared_ptr<GeomAPI_Shape> > aPreviewMap;

  boost::shared_ptr<SketchPlugin_Feature> aFeature;

  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  if (!aData->isValid())
    return aPreviewMap;
  boost::shared_ptr<ModelAPI_AttributeRefList> aRefList =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefList>(aData->attribute(SKETCH_ATTR_FEATURES));

  std::list<FeaturePtr > aFeatures = aRefList->list();
  std::list<FeaturePtr >::const_iterator anIt = aFeatures.begin(),
                                                                  aLast = aFeatures.end();
  for (; anIt != aLast; anIt++) {
    aFeature = boost::dynamic_pointer_cast<SketchPlugin_Feature>(*anIt);
    boost::shared_ptr<GeomAPI_Shape> aPreview = aFeature->preview();
    if (aPreview)
      aPreviewMap[aFeature] = aPreview;
  }
  return aPreviewMap;
}

void PartSet_OperationSketch::stopOperation()
{
  PartSet_OperationSketchBase::stopOperation();
  emit featureConstructed(feature(), FM_Hide);
  emit closeLocalContext();
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
    boost::shared_ptr<GeomDataAPI_Dir> aNormal = 
      boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_NORM));
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
  boost::shared_ptr<GeomAPI_Dir> aTempDir(isX ? new GeomAPI_Dir(0, 1, 0) : new GeomAPI_Dir(1, 0, 0));
  boost::shared_ptr<GeomAPI_Dir> aYDir(new GeomAPI_Dir(aNormDir->cross(aTempDir)));
  boost::shared_ptr<GeomAPI_Dir> aXDir(new GeomAPI_Dir(aYDir->cross(aNormDir)));

  boost::shared_ptr<GeomDataAPI_Point> anOrigin = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(aData->attribute(SKETCH_ATTR_ORIGIN));
  anOrigin->setValue(anOrigPnt);
  boost::shared_ptr<GeomDataAPI_Dir> aNormal = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_NORM));
  aNormal->setValue(aNormDir);
  boost::shared_ptr<GeomDataAPI_Dir> aDirX = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRX));
  aDirX->setValue(aXDir);
  boost::shared_ptr<GeomDataAPI_Dir> aDirY = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRY));
  aDirY->setValue(aYDir);
  boost::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();

  flushUpdated();

  emit featureConstructed(feature(), FM_Hide);
  emit closeLocalContext();
  emit planeSelected(aDir->x(), aDir->y(), aDir->z());
}
