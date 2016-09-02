// Name   : SketchAPI_Mirror.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
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
    fillAttribute(theObjects, mirrorList());

    execute();
  }
}

SketchAPI_Mirror::~SketchAPI_Mirror()
{

}

std::list<std::shared_ptr<ModelHighAPI_Interface> > SketchAPI_Mirror::mirrored() const
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
  theDumper << aBase << " = " << aSketchName << ".addMirror(" << aMirrorLine << ", "
            << aMirrorObjects << ")" << std::endl;

  // Dump variables for a list of mirrored features
  theDumper << "[";
  std::list<std::shared_ptr<ModelHighAPI_Interface> > aList = mirrored();
  std::list<std::shared_ptr<ModelHighAPI_Interface> >::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    if (anIt != aList.begin())
      theDumper << ", ";
    theDumper << theDumper.name((*anIt)->feature(), false);
  }
  theDumper << "] = " << theDumper.name(aBase) << ".mirrored()" << std::endl;
}
