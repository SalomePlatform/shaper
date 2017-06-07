// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModelHighAPI_Double.h"

#include <ModelAPI_AttributeDouble.h>
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
ModelHighAPI_Double::ModelHighAPI_Double(double theValue)
: myVariantType(VT_DOUBLE)
, myDouble(theValue)
{
}

ModelHighAPI_Double::ModelHighAPI_Double(const std::string & theValue)
: myVariantType(VT_STRING)
, myString(theValue)
{
}

ModelHighAPI_Double::ModelHighAPI_Double(const char * theValue)
: myVariantType(VT_STRING)
, myString(theValue)
{
}

ModelHighAPI_Double::~ModelHighAPI_Double()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Double::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute) const
{
  switch(myVariantType) {
    case VT_DOUBLE: theAttribute->setValue(myDouble); return;
    case VT_STRING: theAttribute->setText(myString); return;
  }
}
