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

PartSet_SelectionMode PartSet_FeaturePrs::setFeature(FeaturePtr theFeature,
                                                     const PartSet_SelectionMode& theMode)
{
  return SM_FirstPoint;
}
