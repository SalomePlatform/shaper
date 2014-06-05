// File:        PartSet_FeaturePointPrs.h
// Created:     04 Jun 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_FeaturePointPrs.h>
#include <PartSet_Tools.h>

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

PartSet_FeaturePointPrs::PartSet_FeaturePointPrs(FeaturePtr theFeature)
{
}

PartSet_FeaturePointPrs::~PartSet_FeaturePointPrs()
{
}
