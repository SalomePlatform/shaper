// File:        PartSet_FeaturePrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_FeaturePrs.h>
#include <PartSet_Tools.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>

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

void PartSet_FeaturePrs::init(FeaturePtr theFeature)
{
  myFeature = theFeature;
}

FeaturePtr PartSet_FeaturePrs::sketch() const
{
  return mySketch;
}

FeaturePtr PartSet_FeaturePrs::feature() const
{
  return myFeature;
}

void PartSet_FeaturePrs::setConstraints(double theX, double theY,
                                        const PartSet_SelectionMode& theMode)
{
  // find a feature point by the selection mode
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = featurePoint(theMode);

  // get all sketch features. If the point with the given coordinates belong to any sketch feature,
  // the constraint is created between the feature point and the found sketch point
  boost::shared_ptr<ModelAPI_Data> aData = sketch()->data();
  boost::shared_ptr<ModelAPI_AttributeRefList> aRefList =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefList>(aData->attribute(SKETCH_ATTR_FEATURES));

  std::list<FeaturePtr > aFeatures = aRefList->list();
  std::list<FeaturePtr >::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  for (; anIt != aLast; anIt++)
  {
    FeaturePtr aFeature = *anIt;
    boost::shared_ptr<GeomDataAPI_Point2D> aFPoint = PartSet_Tools::findPoint(aFeature, theX, theY);
    if (aFPoint)
      PartSet_Tools::createConstraint(sketch(), aFPoint, aPoint);
  }
}
