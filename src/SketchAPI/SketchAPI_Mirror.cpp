// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "SketchAPI_Mirror.h"
#include <SketchAPI_SketchEntity.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Mirror::SketchAPI_Mirror(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

SketchAPI_Mirror::SketchAPI_Mirror(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_RefAttr & theMirrorLine,
    const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theMirrorLine, mirrorLine());
    setMirrorList(theObjects);
  }
}

SketchAPI_Mirror::~SketchAPI_Mirror()
{
}

void SketchAPI_Mirror::setMirrorList(const std::list<std::shared_ptr<ModelAPI_Object> >& theObjects)
{
  fillAttribute(theObjects, mirrorList());
  execute();
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_Mirror::mirrored() const
{
  std::list<ObjectPtr> aList = mirroredObjects()->list();
  std::list<FeaturePtr> anIntermediate;
  std::list<ObjectPtr>::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
    anIntermediate.push_back(aFeature);
  }
  return SketchAPI_SketchEntity::wrap(anIntermediate);
}

//--------------------------------------------------------------------------------------

void SketchAPI_Mirror::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeRefAttrPtr aMirrorLine = mirrorLine();
  AttributeRefListPtr aMirrorObjects = mirrorList();

  // Check all attributes are already dumped. If not, store the constraint as postponed.
  size_t aFirstNotDumped = theDumper.indexOfFirstNotDumped(aMirrorObjects);
  if (!theDumper.isDumped(aMirrorLine) || aFirstNotDumped == 0) {
    theDumper.postpone(aBase);
    return;
  }


  // the number of dumped aMirrorObjects is not changed, no need to dump anything
  static std::map<FeaturePtr, size_t> aNbDumpedArguments;
  std::map<FeaturePtr, size_t>::iterator aFound = aNbDumpedArguments.find(aBase);
  if (aFound != aNbDumpedArguments.end() && aFound->second == aFirstNotDumped) {
    theDumper.postpone(aBase);
    return;
  }
  else
    aNbDumpedArguments[aBase] = aFirstNotDumped;

  if (theDumper.isDumped(aBase)) {
    // the feature is already dumped, but it was postponed, because of some arguments
    // were not dumped yet, thus, it is necessary to update the list of rotated objects
    theDumper << "\n### Update " << aBase->getKind() << std::endl;
    theDumper << aBase << ".setMirrorList(" << aMirrorObjects << ")" << std::endl;
  }
  else {
    // the feature is not dumped yet, make the full dump
    theDumper << aBase << " = " << aSketchName << ".addMirror(" << aMirrorLine << ", "
              << aMirrorObjects << ")" << std::endl;
  }

  // Dump variables for a list of mirrored features
  theDumper << "[";
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > aList = mirrored();
  std::list<std::shared_ptr<SketchAPI_SketchEntity> >::const_iterator anIt = aList.begin();
  for (size_t anIndex = 0; anIndex < aFirstNotDumped; ++anIndex, ++anIt) {
    if (anIt != aList.begin())
      theDumper << ", ";
    theDumper << (*anIt)->feature();
  }
  theDumper << "] = " << theDumper.name(aBase) << ".mirrored()" << std::endl;

  if (theDumper.isDumped(aMirrorObjects)) {
    aNbDumpedArguments.erase(aBase);
    // Set necessary "auxiliary" flag for mirrored features
    // (flag is set if it differs to base entity)
    std::list<ObjectPtr> aMirList = aMirrorObjects->list();
    std::list<ObjectPtr>::const_iterator aMIt = aMirList.begin();
    for (anIt = aList.begin(); aMIt != aMirList.end(); ++aMIt, ++anIt) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(*aMIt);
      if (!aFeature)
        continue;
      bool aBaseAux = aFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value();

      aFeature = (*anIt)->feature();
      bool aFeatAux = aFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value();
      if (aFeatAux != aBaseAux)
        theDumper << theDumper.name((*anIt)->feature(), false)
                  << ".setAuxiliary(" << aFeatAux << ")" <<std::endl;
    }
  }
  else {
    // If all refereced objects are not dumped yet, mark the feature as postponed.
    theDumper.postpone(aBase);
  }
}
