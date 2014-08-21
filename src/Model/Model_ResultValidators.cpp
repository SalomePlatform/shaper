// File:        Model_ResultValidators.cpp
// Created:     23 July 2014
// Author:      Vitaly SMETANNIKOV

#include "Model_ResultValidators.h"

#include <ModelAPI_Result.h>
#include <ModelAPI_Tools.h>
#include <GeomAPI_Shape.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <GeomAdaptor_Curve.hxx>

ResultPtr result(const ObjectPtr theObject)
{
  return boost::dynamic_pointer_cast<ModelAPI_Result>(theObject);
}

TopoDS_Shape shape(ResultPtr theResult)
{
  boost::shared_ptr<GeomAPI_Shape> aShape = ModelAPI_Tools::shape(theResult);
  if (aShape)
    return aShape->impl<TopoDS_Shape>();
  return TopoDS_Shape();
}

bool Model_ResultPointValidator::isValid(const ObjectPtr theObject) const
{
  ResultPtr aResult = result(theObject);
  if (!aResult)
    return false;
  TopoDS_Shape aShape = shape(aResult);
  if (aShape.IsNull())
    return false;

  return aShape.ShapeType() == TopAbs_VERTEX;
}

bool Model_ResultLineValidator::isValid(const ObjectPtr theObject) const
{
  ResultPtr aResult = result(theObject);
  if (!aResult)
    return false;
  TopoDS_Shape aShape = shape(aResult);
  if (aShape.IsNull())
    return false;

  if (aShape.ShapeType() == TopAbs_EDGE) {
    TopoDS_Edge aEdge = TopoDS::Edge(aShape);
    Standard_Real aStart, aEnd;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge, aStart, aEnd);
    GeomAdaptor_Curve aAdaptor(aCurve);
    return aAdaptor.GetType() == GeomAbs_Line;
  }
  return false;
}

bool Model_ResultArcValidator::isValid(const ObjectPtr theObject) const
{
  ResultPtr aResult = result(theObject);
  if (!aResult)
    return false;
  TopoDS_Shape aShape = shape(aResult);
  if (aShape.IsNull())
    return false;

  TopAbs_ShapeEnum aa = aShape.ShapeType();
  if (aShape.ShapeType() == TopAbs_EDGE) {
    TopoDS_Edge aEdge = TopoDS::Edge(aShape);
    Standard_Real aStart, aEnd;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge, aStart, aEnd);
    GeomAdaptor_Curve aAdaptor(aCurve);
    return aAdaptor.GetType() == GeomAbs_Circle;
  }
  return false;
}

