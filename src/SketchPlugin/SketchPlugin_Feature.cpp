// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

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

void SketchPlugin_Feature::erase()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch)
    aSketch->removeFeature(this);

  ModelAPI_Feature::erase();
}

SketchPlugin_Sketch* SketchPlugin_Feature::sketch()
{
  if (!mySketch) {
    // find sketch that references to this feature
    const std::set<AttributePtr>& aBackRefs = data()->refsToMe();
    std::set<AttributePtr>::const_iterator aBackRef = aBackRefs.begin();
    for(; aBackRef != aBackRefs.end(); aBackRef++) {
      std::shared_ptr<SketchPlugin_Sketch> aSketch = 
        std::dynamic_pointer_cast<SketchPlugin_Sketch>((*aBackRef)->owner());
      if (aSketch) {
        mySketch = aSketch.get();
        break;
      }
    }
  }
  return mySketch;
}

