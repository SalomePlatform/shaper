// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "SketchPlugin_Feature.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>

/// It is important.
///
/// Before writing a new method implementation in this file, please check the next rule:
/// exported public methods must not be implemented in this source file. They should be inline and
/// placed in the header file.
/// Because it leads to the runtime problem on the Linux OS.
///
/// The reason is that this is an abstract interface. An interface of this class can be used in
/// outside libraries through casting without a link to the current library.

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

