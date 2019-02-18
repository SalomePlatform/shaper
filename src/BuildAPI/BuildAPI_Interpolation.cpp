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

#include "BuildAPI_Interpolation.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Interpolation::BuildAPI_Interpolation(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Interpolation::BuildAPI_Interpolation(const FeaturePtr& theFeature,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                    const ModelHighAPI_Selection& theStartTangent,
                    const ModelHighAPI_Selection& theEndTangent,
                    const bool theIsClosed,
                    const bool theIsToReorder)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setUseTangents(true);
    setTangents(theStartTangent, theEndTangent);
    setClosed(theIsClosed);
    setReorder(theIsToReorder);
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Interpolation::BuildAPI_Interpolation(const FeaturePtr& theFeature,
  const std::list<ModelHighAPI_Selection>& theBaseObjects,
  const bool theIsClosed,
  const bool theIsToReorder)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setClosed(theIsClosed);
    setReorder(theIsToReorder);
    setUseTangents(false);
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Interpolation::~BuildAPI_Interpolation()
{
}

//==================================================================================================
void BuildAPI_Interpolation::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void BuildAPI_Interpolation::setClosed(const bool theIsClosed)
{
  fillAttribute(theIsClosed, myclosed);

  execIfBaseNotEmpty();
}

void BuildAPI_Interpolation::setReorder(const bool theIsToReorder)
{
  fillAttribute(theIsToReorder, myreorder);

  execIfBaseNotEmpty();
}

void BuildAPI_Interpolation::setUseTangents(const bool theIsToUseTangents)
{
  fillAttribute(theIsToUseTangents ? "true" : "", myuseTangents);

  execIfBaseNotEmpty();
}

void BuildAPI_Interpolation::setTangents(const ModelHighAPI_Selection& theStartTangent,
                                         const ModelHighAPI_Selection& theEndTangent)
{
  fillAttribute(theStartTangent, mystartTangent);
  fillAttribute(theEndTangent, myendTangent);

  execIfBaseNotEmpty();
}

//==================================================================================================
void BuildAPI_Interpolation::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrBaseObjects =
    aBase->selectionList(BuildPlugin_Interpolation::BASE_OBJECTS_ID());

  theDumper << aBase << " = model.addInterpolation(" << aPartName << ", "
            << anAttrBaseObjects << ", ";

  AttributeStringPtr useTangentsAttr = useTangents();
  std::string useTangents = useTangentsAttr->value();
  if (!useTangents.empty()) {
    AttributeSelectionPtr anAttrStartTangent =
      aBase->selection(BuildPlugin_Interpolation::TANGENT_START_ID());
    AttributeSelectionPtr anAttrEndTangent =
      aBase->selection(BuildPlugin_Interpolation::TANGENT_END_ID());

    theDumper << anAttrStartTangent << ", " << anAttrEndTangent << ", ";
  }

  theDumper << closed() << ", " << reorder() << ")" << std::endl;
}

//==================================================================================================
InterpolationPtr addInterpolation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const bool theIsClosed,
                                  const bool theIsToReorder)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Interpolation::ID());
  return InterpolationPtr(new BuildAPI_Interpolation(aFeature,
                                                     theBaseObjects,
                                                     theIsClosed,
                                                     theIsToReorder));
}

//==================================================================================================
InterpolationPtr addInterpolation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theStartTangent,
                                  const ModelHighAPI_Selection& theEndTangent,
                                  const bool theIsClosed,
                                  const bool theIsToReorder)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Interpolation::ID());
  return InterpolationPtr(new BuildAPI_Interpolation(aFeature,
                                                     theBaseObjects,
                                                     theStartTangent,
                                                     theEndTangent,
                                                     theIsClosed,
                                                     theIsToReorder));
}

//==================================================================================================
void BuildAPI_Interpolation::execIfBaseNotEmpty()
{
  if (baseObjects()->size() > 0)
    execute();
}