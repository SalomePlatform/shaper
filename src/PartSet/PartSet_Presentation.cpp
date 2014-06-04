// File:        PartSet_Presentation.h
// Created:     02 June 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Presentation.h>
#include <PartSet_Tools.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Constraint.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_Shape.hxx>

#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

const Quantity_NameOfColor SKETCH_PLANE_COLOR = Quantity_NOC_CHOCOLATE; /// the plane edge color
const int SKETCH_WIDTH = 4; /// the plane edge width

Handle(AIS_InteractiveObject) PartSet_Presentation::createPresentation(
                                         boost::shared_ptr<ModelAPI_Feature> theFeature,
                                         boost::shared_ptr<ModelAPI_Feature> theSketch,
                                         const TopoDS_Shape& theShape,
                                         Handle_AIS_InteractiveObject thePrevPrs)
{
  Handle(AIS_InteractiveObject) anAIS;

  if (theFeature->getKind() == "SketchConstraintLength")
    anAIS = createSketchConstraintLength(theFeature, theSketch, thePrevPrs);
  else {
    anAIS = createFeature(theFeature, theShape, thePrevPrs);
    if (theFeature->getKind() == "Sketch")
    {
      Handle(AIS_Shape) aShapeAIS = Handle(AIS_Shape)::DownCast(anAIS);
      aShapeAIS->SetColor(Quantity_Color(SKETCH_PLANE_COLOR));
      aShapeAIS->SetWidth(SKETCH_WIDTH);
      aShapeAIS->Redisplay();
    }
  }

  return anAIS;
}

Handle(AIS_InteractiveObject) PartSet_Presentation::createFeature(
                                              boost::shared_ptr<ModelAPI_Feature> theFeature,
                                              const TopoDS_Shape& theShape,
                                              Handle_AIS_InteractiveObject thePrevPrs)
{
  Handle(AIS_InteractiveObject) anAIS = thePrevPrs;
  if (!anAIS.IsNull())
  {
    Handle(AIS_Shape) aShapeAIS = Handle(AIS_Shape)::DownCast(anAIS);
    if (!aShapeAIS.IsNull()) {
      // if the AIS object is displayed in the opened local context in some mode, additional
      // AIS sub objects are created there. They should be rebuild for correct selecting.
      // It is possible to correct it by closing local context before the shape set and opening
      // after. Another workaround to thrown down the selection and reselecting the AIS.
      // If there was a problem here, try the first solution with close/open local context.
      aShapeAIS->Set(theShape);
      aShapeAIS->Redisplay(Standard_True);
    }
  }
  else
  {
    anAIS = new AIS_Shape(theShape);
  }
  return anAIS;
}

Handle(AIS_InteractiveObject) PartSet_Presentation::createSketchConstraintLength(
                                         boost::shared_ptr<ModelAPI_Feature> theFeature,
                                         boost::shared_ptr<ModelAPI_Feature> theSketch,
                                         Handle(AIS_InteractiveObject) thePrevPrs)
{
  if (!theFeature || !theSketch)
    return thePrevPrs;

  boost::shared_ptr<ModelAPI_Data> aData = theSketch->data();
  boost::shared_ptr<GeomDataAPI_Point> anOrigin = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(aData->attribute(SKETCH_ATTR_ORIGIN));
  boost::shared_ptr<GeomDataAPI_Dir> aNormal = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aData->attribute(SKETCH_ATTR_NORM));
  gp_Pln aPlane(aNormal->x(), aNormal->y(), aNormal->z(), 0/*D*/);

  aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  if (!anAttr)
    return thePrevPrs;
  boost::shared_ptr<ModelAPI_Feature> aFeature = anAttr->feature();
  if (!aFeature || aFeature->getKind() != "SketchLine")
    return thePrevPrs;

  aData = aFeature->data();
  if (!aData->isValid())
    return thePrevPrs;

  boost::shared_ptr<GeomDataAPI_Point2D> aPointStart =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
  boost::shared_ptr<GeomDataAPI_Point2D> aPointEnd =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));

  gp_Pnt aPoint1, aPoint2;
  PartSet_Tools::ConvertTo3D(aPointStart->x(), aPointStart->y(), theSketch, aPoint1);
  PartSet_Tools::ConvertTo3D(aPointEnd->x(), aPointEnd->y(), theSketch, aPoint2);

  //Build dimension here
  gp_Pnt aP1 = aPoint1;
  gp_Pnt aP2 = aPoint2;

  Handle(AIS_InteractiveObject) anAIS = thePrevPrs;
  if (anAIS.IsNull())
  {
    Handle(AIS_LengthDimension) aLenDim = new AIS_LengthDimension (aP1, aP2, aPlane);

    Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
    anAspect->MakeArrows3d (Standard_False);
    anAspect->MakeText3d(false/*is text 3d*/);
    anAspect->TextAspect()->SetHeight(28);
    anAspect->MakeTextShaded(false/*is test shaded*/);
    aLenDim->DimensionAspect()->MakeUnitsDisplayed(false/*is units displayed*/);
    /*if (isUnitsDisplayed)
    {
      aLenDim->SetDisplayUnits (aDimDlg->GetUnits ());
    }*/
    aLenDim->SetDimensionAspect (anAspect);
    aLenDim->SetFlyout(12);

    anAIS = aLenDim;
  }
  else {
    // update presentation
    Handle(AIS_LengthDimension) aDimAIS = Handle(AIS_LengthDimension)::DownCast(anAIS);
    if (!aDimAIS.IsNull()) {
      aDimAIS->SetMeasuredGeometry(aPoint1, aPoint2, aPlane);
      aDimAIS->Redisplay(Standard_True);
    }
  }
  return anAIS;
}
