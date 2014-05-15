#include "SketchPlugin_Feature.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeRefList.h>

SketchPlugin_Feature::SketchPlugin_Feature()
{
  mySketch = 0;
}

void SketchPlugin_Feature::setData(boost::shared_ptr<ModelAPI_Data> theData)
{
  ModelAPI_Feature::setData(theData);

  // find sketch that references to this feature
  int aSketches = document()->size("Construction");
  for(int a = 0; a < aSketches && !mySketch; a++) {
    boost::shared_ptr<SketchPlugin_Sketch> aSketch = 
      boost::dynamic_pointer_cast<SketchPlugin_Sketch>(document()->feature("Construction", a));
    std::list<boost::shared_ptr<ModelAPI_Feature> > aList = 
      aSketch->data()->reflist(SKETCH_ATTR_FEATURES)->list();
    std::list<boost::shared_ptr<ModelAPI_Feature> >::iterator aSub = aList.begin();
    for(; aSub != aList.end(); aSub++) {
      if ((*aSub)->data()->isEqual(theData)) {
        mySketch = aSketch.get();
        break;
      }
    }
  }
}

void SketchPlugin_Feature::setPreview(const boost::shared_ptr<GeomAPI_Shape>& theShape)
{
  myPreview = theShape;
}

const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Feature::getPreview() const
{
  return myPreview;
}
