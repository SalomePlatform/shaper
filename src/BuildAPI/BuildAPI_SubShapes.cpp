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

#include "BuildAPI_SubShapes.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_SubShapes::BuildAPI_SubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_SubShapes::BuildAPI_SubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theBaseShape,
                                       const std::list<ModelHighAPI_Selection>& theSubShapes)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseShape, mybaseShape);
    setSubShapes(theSubShapes);
  }
}

//==================================================================================================
BuildAPI_SubShapes::~BuildAPI_SubShapes()
{

}

//==================================================================================================
void BuildAPI_SubShapes::setBaseShape(const ModelHighAPI_Selection& theBaseShape)
{
  fillAttribute(theBaseShape, mybaseShape);

  execute();
}

//==================================================================================================
void BuildAPI_SubShapes::setSubShapes(const std::list<ModelHighAPI_Selection>& theSubShapes)
{
  fillAttribute(theSubShapes, mysubShapes);

  execute();
}

//==================================================================================================
void BuildAPI_SubShapes::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addSubShapes(" << aPartName << ", "
            << aBase->selection(BuildPlugin_SubShapes::BASE_SHAPE_ID()) << ", "
            << aBase->selectionList(BuildPlugin_SubShapes::SUBSHAPES_ID()) << ")" << std::endl;
}

//==================================================================================================
SubShapesPtr addSubShapes(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Selection& theBaseShape,
                          const std::list<ModelHighAPI_Selection>& theSubShapes)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_SubShapes::ID());
  return SubShapesPtr(new BuildAPI_SubShapes(aFeature, theBaseShape, theSubShapes));
}
