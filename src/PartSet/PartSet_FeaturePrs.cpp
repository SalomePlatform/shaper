// File:        PartSet_FeaturePrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_FeaturePrs.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Constraint.h>

#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>

#include <Precision.hxx>

using namespace std;

PartSet_FeaturePrs::PartSet_FeaturePrs(FeaturePtr theFeature)
: mySketch(theFeature)
{
}

PartSet_FeaturePrs::~PartSet_FeaturePrs()
{
}

void PartSet_FeaturePrs::init(FeaturePtr theFeature, FeaturePtr theSourceFeature)
{
  myFeature = theFeature;
  if (theSourceFeature)
  {
    // use the last point of the previous feature as the first of the new one
    boost::shared_ptr<ModelAPI_Data> aData = theSourceFeature->data();
    boost::shared_ptr<GeomDataAPI_Point2D> anInitPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                                  (aData->attribute(LINE_ATTR_END));
    setLinePoint(theFeature, anInitPoint->x(), anInitPoint->y(), LINE_ATTR_START);
    setLinePoint(theFeature, anInitPoint->x(), anInitPoint->y(), LINE_ATTR_END);

    aData = theFeature->data();
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                                 (aData->attribute(LINE_ATTR_START));
    createConstraint(anInitPoint, aPoint);
  }
}

boost::shared_ptr<ModelAPI_Document> PartSet_FeaturePrs::document() const
{
  return ModelAPI_PluginManager::get()->rootDocument();
}

FeaturePtr PartSet_FeaturePrs::sketch() const
{
  return mySketch;
}

PartSet_SelectionMode PartSet_FeaturePrs::setPoint(double theX, double theY,
                                                   const PartSet_SelectionMode& theMode)
{
  PartSet_SelectionMode aMode = theMode;
  switch (theMode)
  {
    case SM_FirstPoint: {
      setLinePoint(feature(), theX, theY, LINE_ATTR_START);
      setLinePoint(feature(), theX, theY, LINE_ATTR_END);
      aMode = SM_SecondPoint;
    }
    break;
    case SM_SecondPoint: {
      setLinePoint(feature(), theX, theY, LINE_ATTR_END);
      aMode = SM_DonePoint;
   }
    break;
    default:
      break;
  }
  return aMode;
}

FeaturePtr PartSet_FeaturePrs::feature() const
{
  return myFeature;
}

void PartSet_FeaturePrs::createConstraint(boost::shared_ptr<GeomDataAPI_Point2D> thePoint1,
                                          boost::shared_ptr<GeomDataAPI_Point2D> thePoint2)
{
  boost::shared_ptr<ModelAPI_Document> aDoc = document();
  FeaturePtr aFeature = aDoc->addFeature(SKETCH_CONSTRAINT_COINCIDENCE_KIND);

  if (sketch()) {
    boost::shared_ptr<SketchPlugin_Feature> aSketch = 
                           boost::dynamic_pointer_cast<SketchPlugin_Feature>(sketch());
    aSketch->addSub(aFeature);
  }

  boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();

  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef1 =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  aRef1->setAttr(thePoint1);

  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef2 =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_B));
  aRef2->setAttr(thePoint2);

  if (aFeature) // TODO: generate an error if feature was not created
    aFeature->execute();
}

void PartSet_FeaturePrs::setConstraints(double theX, double theY,
                                        const PartSet_SelectionMode& theMode)
{
  std::string aPointArg;
  switch (theMode)
  {
    case SM_FirstPoint:
      aPointArg = LINE_ATTR_START;
      break;
    case SM_SecondPoint:
      aPointArg = LINE_ATTR_END;
      break;
    default:
      break;
  }

  FeaturePtr aSkFeature = feature();

  boost::shared_ptr<ModelAPI_Data> aData = feature()->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                              (aData->attribute(aPointArg));
  aData = sketch()->data();
  boost::shared_ptr<ModelAPI_AttributeRefList> aRefList =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefList>(aData->attribute(SKETCH_ATTR_FEATURES));

  std::list<FeaturePtr > aFeatures = aRefList->list();
  std::list<FeaturePtr >::const_iterator anIt = aFeatures.begin(),
                                                                  aLast = aFeatures.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    boost::shared_ptr<GeomDataAPI_Point2D> aFPoint = findLinePoint(aFeature, theX, theY);
    if (aFPoint)
      createConstraint(aFPoint, aPoint);
  }
}

std::string PartSet_FeaturePrs::getAttribute(const PartSet_SelectionMode& theMode) const
{
  std::string aAttribute;
  switch (theMode)
  {
    case SM_FirstPoint:
      aAttribute = LINE_ATTR_START;
    break;
    case SM_SecondPoint:
      aAttribute = LINE_ATTR_END;
    break;
    default:
    break;
  }
  return aAttribute;
}

PartSet_SelectionMode PartSet_FeaturePrs::getNextMode(const std::string& theAttribute) const
{
  PartSet_SelectionMode aMode;

  if (theAttribute == LINE_ATTR_START)
    aMode = SM_SecondPoint;
  else if (theAttribute == LINE_ATTR_END)
    aMode = SM_DonePoint;
  return aMode;
}

void PartSet_FeaturePrs::getLinePoint(FeaturePtr theFeature,
                                               const std::string& theAttribute,
                                               double& theX, double& theY)
{
  if (!theFeature || theFeature->getKind() != SKETCH_LINE_KIND)
    return;
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));
  theX = aPoint->x();
  theY = aPoint->y();
}

boost::shared_ptr<GeomDataAPI_Point2D> PartSet_FeaturePrs::findLinePoint(
                                               FeaturePtr theFeature,
                                               double theX, double theY)
{
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D;
  if (!theFeature || theFeature->getKind() != SKETCH_LINE_KIND)
    return aPoint2D;
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
  if (fabs(aPoint->x() - theX) < Precision::Confusion() && fabs(aPoint->y() - theY) < Precision::Confusion() )
    aPoint2D = aPoint;
  else {
    aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));
    if (fabs(aPoint->x() - theX) < Precision::Confusion() && fabs(aPoint->y() - theY) < Precision::Confusion() )
      aPoint2D = aPoint;
  }
  return aPoint2D;
}

void PartSet_FeaturePrs::setLinePoint(FeaturePtr theFeature,
                                               double theX, double theY,
                                               const std::string& theAttribute)
{
  if (!theFeature)
    return;
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(theAttribute));
  aPoint->setValue(theX, theY);
}
