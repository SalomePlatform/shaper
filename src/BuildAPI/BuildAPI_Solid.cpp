// Copyright (C) 2017-2023  CEA, EDF
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

#include "BuildAPI_Solid.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Solid::BuildAPI_Solid(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Solid::BuildAPI_Solid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Solid::~BuildAPI_Solid()
{
}

//==================================================================================================
void BuildAPI_Solid::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);
  execute();
}

//==================================================================================================
void BuildAPI_Solid::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addSolid(" << aPartName << ", "
            << aBase->selectionList(BuildPlugin_Solid::BASE_OBJECTS_ID()) << ")" << std::endl;
}

//==================================================================================================
SolidPtr addSolid(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Solid::ID());
  return SolidPtr(new BuildAPI_Solid(aFeature, theBaseObjects));
}
