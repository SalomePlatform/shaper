// Copyright (C) 2019-2023  CEA, EDF
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

#include <PlaneGCSSolver_ScalarArrayWrapper.h>
#include <PlaneGCSSolver_Tools.h>

PlaneGCSSolver_ScalarArrayWrapper::PlaneGCSSolver_ScalarArrayWrapper(const GCS::VEC_pD& theParam)
  : myValue(theParam)
{
}

bool PlaneGCSSolver_ScalarArrayWrapper::update(AttributePtr theAttribute)
{
  bool isUpdated = false;
  PlaneGCSSolver_Tools::AttributeArray anArray(theAttribute);
  if (anArray.isInitialized() && anArray.size() == (int)myValue.size()) {
    for (int anIndex = 0; anIndex < anArray.size(); ++anIndex) {
      isUpdated = PlaneGCSSolver_Tools::updateValue(anArray.value(anIndex), *(myValue[anIndex]))
          || isUpdated;
    }
  }
  return isUpdated;
}
