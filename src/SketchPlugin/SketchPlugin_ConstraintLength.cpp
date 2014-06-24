// File:    SketchPlugin_ConstraintLength.cpp
// Created: 30 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintLength.h"
#include <SketchPlugin_Line.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <GeomDataAPI_Point2D.h>

#include <AIS_LengthDimension.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

SketchPlugin_ConstraintLength::SketchPlugin_ConstraintLength()
{
}

void SketchPlugin_ConstraintLength::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_VALUE,    ModelAPI_AttributeDouble::type());
  data()->addAttribute(CONSTRAINT_ATTR_FLYOUT_VALUE, ModelAPI_AttributeDouble::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintLength::execute()
{
}

Handle(AIS_InteractiveObject) SketchPlugin_ConstraintLength::getAISShape(
  Handle_AIS_InteractiveObject thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();

  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(data()->attribute(CONSTRAINT_ATTR_ENTITY_A));
  if (!anAttr)
    return thePrevious;
  FeaturePtr aFeature = anAttr->feature();
  if (!aFeature || aFeature->getKind() != SKETCH_LINE_KIND)
    return thePrevious;

  boost::shared_ptr<ModelAPI_AttributeDouble> aFlyoutAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE));
  double aFlyout = aFlyoutAttr->value();

  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(CONSTRAINT_ATTR_VALUE));
  double aValue = aValueAttr->value();

  boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();
  if (!aData->isValid())
    return thePrevious;

  boost::shared_ptr<GeomDataAPI_Point2D> aPointStart =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
  boost::shared_ptr<GeomDataAPI_Point2D> aPointEnd =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));

  //Build dimension here
  boost::shared_ptr<GeomAPI_Pnt> aPoint1 = sketch()->to3D(aPointStart->x(), aPointStart->y());
  boost::shared_ptr<GeomAPI_Pnt> aPoint2 = sketch()->to3D(aPointEnd->x(),   aPointEnd->y());
  if (aFlyout < 0)
    aPoint1.swap(aPoint2);

  Handle(AIS_InteractiveObject) anAIS = thePrevious;
  if (anAIS.IsNull())
  {
    Handle(AIS_LengthDimension) aDimAIS = 
      new AIS_LengthDimension(aPoint1->impl<gp_Pnt>(), aPoint2->impl<gp_Pnt>(), aPlane->impl<gp_Pln>());
    aDimAIS->SetCustomValue(aValue);

    Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
    anAspect->MakeArrows3d (Standard_False);
    anAspect->MakeText3d(false/*is text 3d*/);
    anAspect->TextAspect()->SetHeight(CONSTRAINT_TEXT_HEIGHT);
    anAspect->MakeTextShaded(false/*is test shaded*/);
    aDimAIS->DimensionAspect()->MakeUnitsDisplayed(false/*is units displayed*/);
    /*if (isUnitsDisplayed)
    {
      aDimAIS->SetDisplayUnits (aDimDlg->GetUnits ());
    }*/
    aDimAIS->SetDimensionAspect (anAspect);
    aDimAIS->SetFlyout(aFlyout);
    aDimAIS->SetSelToleranceForText2d(CONSTRAINT_TEXT_SELECTION_TOLERANCE);

    anAIS = aDimAIS;
  }
  else
  {
    // update presentation
    Handle(AIS_LengthDimension) aDimAIS = Handle(AIS_LengthDimension)::DownCast(anAIS);
    if (!aDimAIS.IsNull()) 
    {
      aDimAIS->SetMeasuredGeometry(aPoint1->impl<gp_Pnt>(), aPoint2->impl<gp_Pnt>(), aPlane->impl<gp_Pln>());
      aDimAIS->SetCustomValue(aValue);
      aDimAIS->SetFlyout(aFlyout);

      aDimAIS->Redisplay(Standard_True);
    }
  }
  return anAIS;
}

