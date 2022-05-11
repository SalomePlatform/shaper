// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "SketchAPI_Offset.h"
#include <SketchAPI_SketchEntity.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//--------------------------------------------------------------------------------------
SketchAPI_Offset::SketchAPI_Offset (const std::shared_ptr<ModelAPI_Feature> & theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

SketchAPI_Offset::SketchAPI_Offset (const std::shared_ptr<ModelAPI_Feature> & theFeature,
                                    const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
                                    const ModelHighAPI_Double & theOffsetValue,
                                    const bool theIsReversed,
                                    const std::string & theJointType)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theObjects, edgesList());
    fillAttribute(theOffsetValue, value());
    fillAttribute(theIsReversed, reversed());
    fillAttribute(theJointType, joint());

    execute();
  }
}

SketchAPI_Offset::~SketchAPI_Offset()
{
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_Offset::offset() const
{
  std::list<ObjectPtr> aList = feature()->reflist(SketchPlugin_Constraint::ENTITY_B())->list();
  std::list<FeaturePtr> anIntermediate;
  std::list<ObjectPtr>::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
    anIntermediate.push_back(aFeature);
  }
  return SketchAPI_SketchEntity::wrap(anIntermediate);
}

//--------------------------------------------------------------------------------------

void SketchAPI_Offset::dump (ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeRefListPtr aOffsetObjects = edgesList();
  AttributeDoublePtr aValue = value();
  AttributeBooleanPtr aReversed = reversed();
  AttributeStringPtr aJoint = joint();

  // Check all attributes are already dumped. If not, store the feature as postponed.
  if (!theDumper.isDumped(aOffsetObjects)) {
    theDumper.postpone(aBase);
    return;
  }

  theDumper << aBase << " = " << aSketchName << ".addOffset(" << aOffsetObjects << ", "
            << aValue << ", " << aReversed  << ", " << aJoint << ")" << std::endl;

  // Dump variables for a list of created features
  theDumper << "[";
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > aList = offset();
  std::list<std::shared_ptr<SketchAPI_SketchEntity> >::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    if (anIt != aList.begin())
      theDumper << ", ";
    theDumper << (*anIt)->feature();
  }
  theDumper << "] = " << theDumper.name(aBase) << ".offset()" << std::endl;

  // Set necessary "auxiliary" flag for created features
  for (anIt = aList.begin(); anIt != aList.end(); ++anIt) {
    FeaturePtr aFeature = (*anIt)->feature();
    bool aFeatAux = aFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value();
    if (aFeatAux)
      theDumper << theDumper.name((*anIt)->feature(), false)
                << ".setAuxiliary(" << aFeatAux << ")" <<std::endl;
  }
}
