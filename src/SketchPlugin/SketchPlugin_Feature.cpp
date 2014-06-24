#include "SketchPlugin_Feature.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeRefList.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

SketchPlugin_Feature::SketchPlugin_Feature()
{
  mySketch = 0;
}

void SketchPlugin_Feature::setPreview(const boost::shared_ptr<GeomAPI_Shape>& theShape)
{
  myPreview = theShape;
}

const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Feature::getPreview() const
{
  return myPreview;
}

SketchPlugin_Sketch* SketchPlugin_Feature::sketch()
{
  if (!mySketch) {
    // find sketch that references to this feature
    int aSketches = document()->size("Construction");
    for(int a = 0; a < aSketches && !mySketch; a++) {
      boost::shared_ptr<SketchPlugin_Sketch> aSketch = boost::
        dynamic_pointer_cast<SketchPlugin_Sketch>(document()->feature("Construction", a, true));
      if (aSketch) {
        std::list<FeaturePtr > aList = 
          aSketch->data()->reflist(SKETCH_ATTR_FEATURES)->list();
        std::list<FeaturePtr >::iterator aSub = aList.begin();
        for(; aSub != aList.end(); aSub++) {
          if ((*aSub)->data()->isEqual(data())) {
            mySketch = aSketch.get();
            break;
          }
        }
      }
    }
  }
  return mySketch;
}

Handle(AIS_InteractiveObject) SketchPlugin_Feature::getAISShape(Handle(AIS_InteractiveObject) thePrevious)
{
  boost::shared_ptr<GeomAPI_Shape> aPreview = preview();

  Handle(AIS_InteractiveObject) anAIS = thePrevious;
  const TopoDS_Shape& aShape = aPreview ? aPreview->impl<TopoDS_Shape>() : TopoDS_Shape();
  if (!anAIS.IsNull())
  {
    Handle(AIS_Shape) aShapeAIS = Handle(AIS_Shape)::DownCast(anAIS);
    if (!aShapeAIS.IsNull()) {
      // if the AIS object is displayed in the opened local context in some mode, additional
      // AIS sub objects are created there. They should be rebuild for correct selecting.
      // It is possible to correct it by closing local context before the shape set and opening
      // after. Another workaround to thrown down the selection and reselecting the AIS.
      // If there was a problem here, try the first solution with close/open local context.

      aShapeAIS->Set(aShape);
      aShapeAIS->Redisplay(Standard_True);
    }
  }
  else
  {
    anAIS = new AIS_Shape(aShape);
  }
  return anAIS;
}
