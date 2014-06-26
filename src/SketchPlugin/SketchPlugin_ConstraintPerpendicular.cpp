// File:    SketchPlugin_ConstraintPerpendicular.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintPerpendicular.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Line.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_PerpendicularRelation.hxx>
#include <Geom_Plane.hxx>

SketchPlugin_ConstraintPerpendicular::SketchPlugin_ConstraintPerpendicular()
{
}

void SketchPlugin_ConstraintPerpendicular::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_B, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintPerpendicular::execute()
{
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_ConstraintPerpendicular::preview()
{
  /// \todo Preview for perpendicular constraint
  return getPreview();
}

Handle_AIS_InteractiveObject SketchPlugin_ConstraintPerpendicular::getAISShape(Handle_AIS_InteractiveObject thePrevious)
{
  Handle(AIS_InteractiveObject) anAIS = thePrevious;
  if (!sketch())
    return anAIS;

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr1 = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr2 = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_B));
  if (!anAttr1 || !anAttr1->isFeature() || 
      !anAttr2 || !anAttr2->isFeature())
    return anAIS;
  boost::shared_ptr<SketchPlugin_Line> aLine1Feature = 
    boost::dynamic_pointer_cast<SketchPlugin_Line>(anAttr1->feature());
  boost::shared_ptr<SketchPlugin_Line> aLine2Feature = 
    boost::dynamic_pointer_cast<SketchPlugin_Line>(anAttr2->feature());
  if (!aLine1Feature || !aLine2Feature)
    return anAIS;

  boost::shared_ptr<GeomAPI_Shape> aLine1 = aLine1Feature->preview();
  boost::shared_ptr<GeomAPI_Shape> aLine2 = aLine2Feature->preview();
  Handle(Geom_Plane) aPlane = new Geom_Plane(sketch()->plane()->impl<gp_Pln>());

  if (anAIS.IsNull())
  {
    Handle(AIS_PerpendicularRelation) aPerpendicular = 
      new AIS_PerpendicularRelation(aLine1->impl<TopoDS_Shape>(), aLine2->impl<TopoDS_Shape>(), aPlane);

    anAIS = aPerpendicular;
  }
  else
  {
    Handle(AIS_PerpendicularRelation) aPerpendicular = Handle(AIS_PerpendicularRelation)::DownCast(anAIS);
    if (!aPerpendicular.IsNull())
    {
      aPerpendicular->SetFirstShape(aLine1->impl<TopoDS_Shape>());
      aPerpendicular->SetSecondShape(aLine2->impl<TopoDS_Shape>());
      aPerpendicular->SetPlane(aPlane);
      aPerpendicular->Redisplay(Standard_True);
    }
  }
  return anAIS;
}

