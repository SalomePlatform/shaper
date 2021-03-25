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

#include "BuildAPI_Polyline.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
BuildAPI_Polyline::BuildAPI_Polyline(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
BuildAPI_Polyline::BuildAPI_Polyline(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const bool theIsClosed)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setClosed(theIsClosed);
    setBase(theBaseObjects);
  }
}

//==================================================================================================
BuildAPI_Polyline::~BuildAPI_Polyline()
{

}

//==================================================================================================
void BuildAPI_Polyline::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void BuildAPI_Polyline::setClosed(const bool theIsClosed)
{
  fillAttribute(theIsClosed, myclosed);
}

//==================================================================================================
void BuildAPI_Polyline::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addPolyline3D(" << aPartName << ", "
            << aBase->selectionList(BuildPlugin_Polyline::BASE_OBJECTS_ID()) << ", "
            << closed() << ")" << std::endl;
}

//==================================================================================================
PolylinePtr addPolyline3D(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const bool theIsClosed)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(BuildAPI_Polyline::ID());
  return PolylinePtr(new BuildAPI_Polyline(aFeature, theBaseObjects, theIsClosed));
}
