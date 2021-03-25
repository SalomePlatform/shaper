// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "BuildAPI_Wire.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Wire::BuildAPI_Wire(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Wire::BuildAPI_Wire(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                             const bool theComputeIntersections)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theComputeIntersections, mycomputeIntersections);
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Wire::~BuildAPI_Wire()
{

}

//==================================================================================================
void BuildAPI_Wire::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void BuildAPI_Wire::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addWire(" << aPartName << ", "
            << aBase->selectionList(BuildPlugin_Wire::BASE_OBJECTS_ID());

  AttributeBooleanPtr isIntersect = aBase->boolean(BuildPlugin_Wire::INTERSECT_ID());
  if (isIntersect->isInitialized())
    theDumper << ", " << isIntersect;

  theDumper << ")" << std::endl;
}

//==================================================================================================
void BuildAPI_Wire::addContour()
{
  feature()->customAction(BuildPlugin_Wire::ADD_CONTOUR_ACTION_ID());
}

//==================================================================================================
WirePtr addWire(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const bool theComputeIntersections)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Wire::ID());
  return WirePtr(new BuildAPI_Wire(aFeature, theBaseObjects, theComputeIntersections));
}
