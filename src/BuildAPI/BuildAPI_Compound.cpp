// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "BuildAPI_Compound.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Compound::BuildAPI_Compound(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Compound::BuildAPI_Compound(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Compound::~BuildAPI_Compound()
{
}

//==================================================================================================
void BuildAPI_Compound::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);
  execute();
}

//==================================================================================================
void BuildAPI_Compound::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addCompound(" << aPartName << ", "
            << aBase->selectionList(BuildPlugin_Compound::BASE_OBJECTS_ID()) << ")" << std::endl;
}

//==================================================================================================
CompoundPtr addCompound(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Compound::ID());
  return CompoundPtr(new BuildAPI_Compound(aFeature, theBaseObjects));
}
