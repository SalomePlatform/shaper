// File:    SketchPlugin_ConstraintParallel.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintParallel.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Line.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_ParallelRelation.hxx>
#include <Geom_Plane.hxx>

SketchPlugin_ConstraintParallel::SketchPlugin_ConstraintParallel()
{
}

void SketchPlugin_ConstraintParallel::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_B, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintParallel::execute()
{
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_ConstraintParallel::preview()
{
  /// \todo Preview for parallel constraint
  return getPreview();
}


Handle_AIS_InteractiveObject SketchPlugin_ConstraintParallel::getAISShape(Handle_AIS_InteractiveObject thePrevious)
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
    Handle(AIS_ParallelRelation) aParallel = 
      new AIS_ParallelRelation(aLine1->impl<TopoDS_Shape>(), aLine2->impl<TopoDS_Shape>(), aPlane);

    anAIS = aParallel;
  }
  else
  {
    Handle(AIS_ParallelRelation) aParallel = Handle(AIS_ParallelRelation)::DownCast(anAIS);
    if (!aParallel.IsNull())
    {
      aParallel->SetFirstShape(aLine1->impl<TopoDS_Shape>());
      aParallel->SetSecondShape(aLine2->impl<TopoDS_Shape>());
      aParallel->SetPlane(aPlane);
      aParallel->Redisplay(Standard_True);
    }
  }
  return anAIS;
}

