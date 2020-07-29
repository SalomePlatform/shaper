// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "ModelHighAPI_Integer.h"

#include <ModelAPI_AttributeInteger.h>

#include <sstream>
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
ModelHighAPI_Integer::ModelHighAPI_Integer(int theValue)
: myVariantType(VT_INT)
, myInt(theValue)
{
}

ModelHighAPI_Integer::ModelHighAPI_Integer(const std::wstring & theValue)
: myVariantType(VT_STRING)
, myString(theValue)
{
}

ModelHighAPI_Integer::ModelHighAPI_Integer(const wchar_t * theValue)
: myVariantType(VT_STRING)
, myString(theValue)
{
}

ModelHighAPI_Integer::~ModelHighAPI_Integer()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Integer::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute) const
{
  switch(myVariantType) {
    case VT_INT: theAttribute->setValue(myInt); return;
    case VT_STRING: theAttribute->setText(myString); return;
  }
}

int ModelHighAPI_Integer::intValue() const
{
  // needed for array of integer, which supports no text
  return myInt;
}

std::wstring ModelHighAPI_Integer::string() const
{
  if (myVariantType == VT_STRING)
    return myString;

  std::wostringstream anOut;
  anOut << myInt;
  return anOut.str();
}
