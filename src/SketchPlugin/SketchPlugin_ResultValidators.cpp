// File:        Model_ResultValidators.cpp
// Created:     23 July 2014
// Author:      Vitaly SMETANNIKOV

#include "SketchPlugin_ResultValidators.h"

#include <ModelAPI_Result.h>
#include <ModelAPI_Tools.h>
#include <GeomAPI_Curve.h>

ResultPtr result(const ObjectPtr theObject)
{
  return boost::dynamic_pointer_cast<ModelAPI_Result>(theObject);
}

bool SketchPlugin_ResultPointValidator::isValid(const ObjectPtr theObject) const
{
  ResultPtr aResult = result(theObject);
  if (!aResult)
    return false;
  boost::shared_ptr<GeomAPI_Shape> aShape = ModelAPI_Tools::shape(aResult);
  return aShape && aShape->isVertex();
}

bool SketchPlugin_ResultLineValidator::isValid(const ObjectPtr theObject) const
{
  ResultPtr aResult = result(theObject);
  if (!aResult)
    return false;
  boost::shared_ptr<GeomAPI_Shape> aShape = ModelAPI_Tools::shape(aResult);
  return aShape && aShape->isEdge() && GeomAPI_Curve(aShape).isLine();

  /*
  if (aShape.ShapeType() == TopAbs_EDGE) {
    TopoDS_Edge aEdge = TopoDS::Edge(aShape);
    Standard_Real aStart, aEnd;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge, aStart, aEnd);
    GeomAdaptor_Curve aAdaptor(aCurve);
    return aAdaptor.GetType() == GeomAbs_Line;
  }
  return false;
  */
}

bool SketchPlugin_ResultArcValidator::isValid(const ObjectPtr theObject) const
{
  ResultPtr aResult = result(theObject);
  if (!aResult)
    return false;
  boost::shared_ptr<GeomAPI_Shape> aShape = ModelAPI_Tools::shape(aResult);
  return aShape && aShape->isEdge() && GeomAPI_Curve(aShape).isCircle();
}

