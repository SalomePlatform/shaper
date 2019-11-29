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

#include "BuildAPI_Edge.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Edge::BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Edge::BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                             const bool theComputeIntersections)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(BuildPlugin_Edge::CREATION_BY_SEGMENTS(), mycreationMethod);
    fillAttribute(theComputeIntersections, mycomputeIntersections);
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Edge::BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_Selection& theFirstPoint,
                             const ModelHighAPI_Selection& theSecondPoint)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(BuildPlugin_Edge::CREATION_BY_POINTS(), mycreationMethod);
    fillAttribute(theFirstPoint, myfirstPoint);
    fillAttribute(theSecondPoint, mysecondPoint);

    execute();
  }
}

//==================================================================================================
BuildAPI_Edge::~BuildAPI_Edge()
{

}

//==================================================================================================
void BuildAPI_Edge::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void BuildAPI_Edge::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addEdge(" << aPartName << ", ";

  AttributeStringPtr aCreationMethod = aBase->string(BuildPlugin_Edge::CREATION_METHOD());
  if (aCreationMethod && aCreationMethod->isInitialized() &&
      aCreationMethod->value() == BuildPlugin_Edge::CREATION_BY_POINTS()) {
    theDumper << aBase->selection(BuildPlugin_Edge::FIRST_POINT()) << ", "
              << aBase->selection(BuildPlugin_Edge::SECOND_POINT());
  }
  else {
    theDumper << aBase->selectionList(BuildPlugin_Edge::BASE_OBJECTS_ID());

    AttributeBooleanPtr isIntersect = aBase->boolean(BuildPlugin_Edge::INTERSECT_ID());
    if (isIntersect->isInitialized())
      theDumper << ", " << isIntersect;
  }
  theDumper << ")" << std::endl;
}

//==================================================================================================
EdgePtr addEdge(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const bool theComputeIntersections)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Edge::ID());
  return EdgePtr(new BuildAPI_Edge(aFeature, theBaseObjects, theComputeIntersections));
}

EdgePtr addEdge(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& theFirstPoint,
                const ModelHighAPI_Selection& theSecondPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Edge::ID());
  return EdgePtr(new BuildAPI_Edge(aFeature, theFirstPoint, theSecondPoint));
}
