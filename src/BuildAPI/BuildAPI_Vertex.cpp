// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include "BuildAPI_Vertex.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Vertex::BuildAPI_Vertex(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Vertex::BuildAPI_Vertex(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(false, mydoIntersect);
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Vertex::BuildAPI_Vertex(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                 const bool theDoIntersect)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theDoIntersect, mydoIntersect);
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Vertex::~BuildAPI_Vertex()
{

}

//==================================================================================================
void BuildAPI_Vertex::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
VertexPtr addVertex(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Vertex::ID());
  return VertexPtr(new BuildAPI_Vertex(aFeature, theBaseObjects));
}

//==================================================================================================
VertexPtr addVertex(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                    const bool theDoIntersect)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Vertex::ID());
  return VertexPtr(new BuildAPI_Vertex(aFeature, theBaseObjects, theDoIntersect));
}

//==================================================================================================
void BuildAPI_Vertex::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addVertex(" << aPartName << ", "
            << aBase->selectionList(BuildPlugin_Vertex::BASE_OBJECTS_ID()) << ", "
            << aBase->boolean(BuildPlugin_Vertex::INTERSECT_ID()) << ")" << std::endl;
}
