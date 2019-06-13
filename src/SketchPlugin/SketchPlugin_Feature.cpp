// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SketchPlugin_Feature.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
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

void SketchPlugin_Feature::keepCurrentFeature()
{
  FeaturePtr aCurFeature = document()->currentFeature(true);
  std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(aCurFeature);
  if (!aSketchFeature || aSketchFeature->sketch() == sketch())
    myCurrentFeature = aCurFeature;
}

void SketchPlugin_Feature::restoreCurrentFeature()
{
  if (myCurrentFeature)
    document()->setCurrentFeature(myCurrentFeature, true);
  myCurrentFeature = FeaturePtr();
}
