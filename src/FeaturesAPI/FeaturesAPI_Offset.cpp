// Copyright (C) 2017-2026  CEA, EDF
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

#include "FeaturesAPI_Offset.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================

FeaturesAPI_Offset::FeaturesAPI_Offset(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

FeaturesAPI_Offset::FeaturesAPI_Offset(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theBaseObject,
                                       const ModelHighAPI_Double& theOffset,
                                       const bool isPipeJoint)
  : FeaturesAPI_Offset(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Offset::CREATION_METHOD_EQUAL(), mycreationMethod);
    fillAttribute(theBaseObject, mybaseObject);
    fillAttribute(theOffset, myoffsetValue);
    fillAttribute(isPipeJoint, myisPipeJoint);

    execute();
  }
}

FeaturesAPI_Offset::FeaturesAPI_Offset(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theBaseObject,
                                       const ModelHighAPI_Double& theOffset,
                                       const std::list<ModelHighAPI_Selection>& theFaces)
  : FeaturesAPI_Offset(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Offset::CREATION_METHOD_PARTIAL(), mycreationMethod);
    fillAttribute(theBaseObject, mybaseObject);
    fillAttribute(theOffset, myoffsetValue);
    fillAttribute(theFaces, myfaces);

    execute();
  }
}

FeaturesAPI_Offset::~FeaturesAPI_Offset()
{
}

void FeaturesAPI_Offset::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrShape =
    aBase->selection(FeaturesPlugin_Offset::BASE_SHAPE_ID());

  AttributeDoublePtr anAttrOffset = aBase->real(FeaturesPlugin_Offset::OFFSET_VALUE_ID());

  std::string aCreationMethod =
    aBase->string(FeaturesPlugin_Offset::CREATION_METHOD_ID())->value();

  if (aCreationMethod == FeaturesPlugin_Offset::CREATION_METHOD_EQUAL()) {
    AttributeBooleanPtr anAttrIsPipe = aBase->boolean(FeaturesPlugin_Offset::PIPE_JOINT_ID());

    theDumper << aBase << " = model.addOffset(" << aDocName << ", " << anAttrShape;
    theDumper << ", " << anAttrOffset << ", " << anAttrIsPipe << ")" << std::endl;
  }
  else if (aCreationMethod == FeaturesPlugin_Offset::CREATION_METHOD_PARTIAL()) {
    AttributeSelectionListPtr anAttrFaces =
      aBase->selectionList(FeaturesPlugin_Offset::FACES_ID());

    theDumper << aBase << " = model.addOffsetPartial(" << aDocName << ", " << anAttrShape;
    theDumper << ", " << anAttrOffset << ", " << anAttrFaces << ")" << std::endl;
  }
}

//==================================================================================================

OffsetPtr addOffset(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theBaseObject,
                    const ModelHighAPI_Double& theOffset,
                    const bool isPipeJoint)
{
  FeaturePtr aFeature = thePart->addFeature(FeaturesAPI_Offset::ID());

  OffsetPtr aOffset (new FeaturesAPI_Offset(aFeature, theBaseObject, theOffset, isPipeJoint));
  return aOffset;
}

OffsetPtr addOffsetPartial(const std::shared_ptr<ModelAPI_Document>& thePart,
                           const ModelHighAPI_Selection& theBaseObject,
                           const ModelHighAPI_Double& theOffset,
                           const std::list<ModelHighAPI_Selection>& theFaces)
{
  FeaturePtr aFeature = thePart->addFeature(FeaturesAPI_Offset::ID());

  OffsetPtr aOffset (new FeaturesAPI_Offset(aFeature, theBaseObject, theOffset, theFaces));
  return aOffset;
}
