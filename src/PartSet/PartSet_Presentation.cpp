// File:        PartSet_Presentation.h
// Created:     02 June 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Presentation.h>
#include <PartSet_Tools.h>

#include <PartSet_FeatureLengthPrs.h>

#include <ModelAPI_Feature.h>

#include <SketchPlugin_Sketch.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>

#include <Quantity_NameOfColor.hxx>

const Quantity_NameOfColor SKETCH_PLANE_COLOR = Quantity_NOC_CHOCOLATE; /// the plane edge color
const int SKETCH_WIDTH = 4; /// the plane edge width

Handle(AIS_InteractiveObject) PartSet_Presentation::createPresentation(
                                         FeaturePtr theFeature,
                                         FeaturePtr theSketch,
                                         const TopoDS_Shape& theShape,
                                         Handle_AIS_InteractiveObject thePreviuos)
{
  Handle(AIS_InteractiveObject) anAIS;

  std::string aKind = theFeature->getKind();
  if (aKind == PartSet_FeatureLengthPrs::getKind())
    anAIS = PartSet_FeatureLengthPrs::createPresentation(theFeature, theSketch, thePreviuos);
  else {
    anAIS = createFeature(theFeature, theShape, thePreviuos);
    if (theFeature->getKind() == SKETCH_KIND)
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
                                              FeaturePtr theFeature,
                                              const TopoDS_Shape& theShape,
                                              Handle_AIS_InteractiveObject thePreviuos)
{
  Handle(AIS_InteractiveObject) anAIS = thePreviuos;
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
