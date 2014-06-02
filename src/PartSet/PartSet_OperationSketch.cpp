// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_OperationSketch.h>

#include <PartSet_OperationEditLine.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Sketch.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>

#include <XGUI_ViewerPrs.h>

#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>
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

std::list<int> PartSet_OperationSketch::getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const
{
  std::list<int> aModes;
  if (!hasSketchPlane())
    aModes.push_back(TopAbs_FACE);
  else
    aModes = PartSet_OperationSketchBase::getSelectionModes(theFeature);
  return aModes;
}

void PartSet_OperationSketch::init(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                   const std::list<XGUI_ViewerPrs>& /*theSelected*/,
                                   const std::list<XGUI_ViewerPrs>& /*theHighlighted*/)
{
  setFeature(theFeature);
}

boost::shared_ptr<ModelAPI_Feature> PartSet_OperationSketch::sketch() const
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
      boost::shared_ptr<ModelAPI_Feature> aFeature = theHighlighted.front().feature();
      if (aFeature)
        restartOperation(PartSet_OperationEditLine::Type(), aFeature);
    }
    else
      myFeatures = theHighlighted;
  }
}

void PartSet_OperationSketch::mouseMoved(QMouseEvent* theEvent, Handle(V3d_View) theView)
{
  if (!hasSketchPlane() || !(theEvent->buttons() &  Qt::LeftButton) || myFeatures.empty())
    return;

  if (myFeatures.size() != 1) {
    boost::shared_ptr<ModelAPI_Feature> aFeature = PartSet_Tools::NearestFeature(theEvent->pos(),
                                                                theView, feature(), myFeatures);
    if (aFeature)
      restartOperation(PartSet_OperationEditLine::Type(), aFeature);
  }
}

std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> >
                                                        PartSet_OperationSketch::subPreview() const
{
  std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> > aPreviewMap;

  boost::shared_ptr<SketchPlugin_Feature> aFeature;

  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  if (!aData->isValid())
    return aPreviewMap;
  boost::shared_ptr<ModelAPI_AttributeRefList> aRefList =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefList>(aData->attribute(SKETCH_ATTR_FEATURES));

  std::list<boost::shared_ptr<ModelAPI_Feature> > aFeatures = aRefList->list();
  std::list<boost::shared_ptr<ModelAPI_Feature> >::const_iterator anIt = aFeatures.begin(),
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
  if (!feature()) {
    setFeature(createFeature());
    emit fitAllView();
  }
}

bool PartSet_OperationSketch::hasSketchPlane() const
{
  bool aHasPlane = false;

  if (feature()) {
    boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
    boost::shared_ptr<ModelAPI_AttributeDouble> anAttr;
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

  boost::shared_ptr<ModelAPI_AttributeDouble> anAttr;
  // temporary solution for main planes only
  boost::shared_ptr<GeomDataAPI_Point> anOrigin = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(aData->attribute(SKETCH_ATTR_ORIGIN));
  anOrigin->setValue(0, 0, 0);
  boost::shared_ptr<GeomDataAPI_Dir> aNormal = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_NORM));
  aNormal->setValue(anA, aB, aC);
  boost::shared_ptr<GeomDataAPI_Dir> aDirX = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRX));
  aDirX->setValue(aB, aC, anA);
  boost::shared_ptr<GeomDataAPI_Dir> aDirY = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_DIRY));
  aDirY->setValue(aC, anA, aB);
  boost::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();

  flushUpdated();

  emit featureConstructed(feature(), FM_Hide);
  emit closeLocalContext();
  emit planeSelected(aDir->x(), aDir->y(), aDir->z());
}
