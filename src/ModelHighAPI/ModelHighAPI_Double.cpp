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

#include "ModelHighAPI_Double.h"

#include <ModelAPI_AttributeDouble.h>
#include <GeomDataAPI_Point.h>

#include <sstream>
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

double ModelHighAPI_Double::value() const
{
  // needed for array of double, which supports no text
  return myDouble;
}

std::string ModelHighAPI_Double::string() const
{
  if (myVariantType == VT_STRING)
    return myString;

  std::ostringstream anOut;
  anOut << myDouble;
  return anOut.str();
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

void ModelHighAPI_Double::fillAttribute(
  const std::shared_ptr<GeomDataAPI_Point> & thePoint,
  const ModelHighAPI_Double & theX,
  const ModelHighAPI_Double & theY,
  const ModelHighAPI_Double & theZ) const
{
  switch (theX.myVariantType) {
  case VT_DOUBLE: thePoint->setX(theX.myDouble); break;
  case VT_STRING: thePoint->setTextX(theX.myString);
  }
  switch (theY.myVariantType) {
  case VT_DOUBLE: thePoint->setY(theY.myDouble); break;
  case VT_STRING: thePoint->setTextY(theY.myString);
  }
  switch (theZ.myVariantType) {
  case VT_DOUBLE: thePoint->setZ(theZ.myDouble); break;
  case VT_STRING: thePoint->setTextZ(theZ.myString);
  }
}
