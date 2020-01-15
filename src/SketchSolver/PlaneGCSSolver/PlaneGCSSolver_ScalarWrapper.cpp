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

#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_Tools.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>

PlaneGCSSolver_ScalarWrapper::PlaneGCSSolver_ScalarWrapper(double *const theParam)
  : myValue(theParam)
{
}

void PlaneGCSSolver_ScalarWrapper::setValue(double theValue)
{
  *myValue = theValue;
}

double PlaneGCSSolver_ScalarWrapper::value() const
{
  return *myValue;
}

bool PlaneGCSSolver_ScalarWrapper::update(AttributePtr theAttribute)
{
  double anAttrValue = 0.0;
  AttributeDoublePtr aDouble =
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (aDouble)
    anAttrValue = aDouble->value();
  else {
    AttributeIntegerPtr anInt =
        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttribute);
    if (anInt)
      anAttrValue = anInt->value();
    else
      return false;
  }

  // There is possible an angular value, which is converted between degrees and radians.
  // So, we use its value instead of using direct pointer to variable.
  double aCurrentValue = value();

  bool isUpdated = PlaneGCSSolver_Tools::updateValue(anAttrValue, aCurrentValue);
  if (isUpdated)
    setValue(aCurrentValue);
  return isUpdated;
}
