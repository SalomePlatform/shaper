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

#include "Model_AttributeBoolean.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

void Model_AttributeBoolean::setValue(bool theValue)
{
  Standard_Integer aValue = theValue ? 1 : 0;
  if (!myIsInitialized || myBool->Get() != aValue) {
    if (myBool.IsNull())
      myBool = TDataStd_Integer::Set(myLab, 0);
    myBool->Set(aValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

bool Model_AttributeBoolean::value()
{
  return myIsInitialized && myBool->Get() == 1;
}

Model_AttributeBoolean::Model_AttributeBoolean(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_AttributeBoolean::reinit()
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = myLab.FindAttribute(TDataStd_Integer::GetID(), myBool) == Standard_True;
}
