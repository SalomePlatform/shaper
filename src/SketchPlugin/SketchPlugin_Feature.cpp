#include "SketchPlugin_Feature.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>

SketchPlugin_Feature::SketchPlugin_Feature()
{
  mySketch = 0;
}

SketchPlugin_Sketch* SketchPlugin_Feature::sketch()
{
  if (!mySketch) {
    // find sketch that references to this feature
    const std::set<AttributePtr>& aBackRefs = data()->refsToMe();
    std::set<AttributePtr>::const_iterator aBackRef = aBackRefs.begin();
    for(; aBackRef != aBackRefs.end(); aBackRef++) {
      boost::shared_ptr<SketchPlugin_Sketch> aSketch = 
        boost::dynamic_pointer_cast<SketchPlugin_Sketch>((*aBackRef)->owner());
      if (aSketch) {
        mySketch = aSketch.get();
        break;
      }
    }
  }
  return mySketch;
}

AISObjectPtr SketchPlugin_Feature::simpleAISObject(boost::shared_ptr<ModelAPI_Result> theRes,
                                                   AISObjectPtr thePrevious)
{
  boost::shared_ptr<ModelAPI_ResultConstruction> aConstr = boost::dynamic_pointer_cast<
      ModelAPI_ResultConstruction>(theRes);

  boost::shared_ptr<GeomAPI_Shape> aPreview;
  if (aConstr)
    aPreview = aConstr->shape();

  AISObjectPtr aResult = thePrevious;
  if (!aResult)
    aResult = AISObjectPtr(new GeomAPI_AISObject());
  aResult->createShape(aPreview);
  return aResult;
}
