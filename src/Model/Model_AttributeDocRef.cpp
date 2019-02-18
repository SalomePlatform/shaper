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

#include "Model_AttributeDocRef.h"
#include "Model_Application.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

void Model_AttributeDocRef::setValue(std::shared_ptr<ModelAPI_Document> theDoc)
{
  if (myID->Get() != theDoc->id()) {
    myID->Set(theDoc->id());
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::shared_ptr<ModelAPI_Document> Model_AttributeDocRef::value()
{
  return Model_Application::getApplication()->document(myID->Get());
}

int Model_AttributeDocRef::docId()
{
  return myID->Get();
}

Model_AttributeDocRef::Model_AttributeDocRef(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_Integer::GetID(), myID) == Standard_True;
  if (!myIsInitialized) {
    int aNewID = Model_Application::getApplication()->generateDocumentId();
    myID = TDataStd_Integer::Set(theLabel, aNewID);
  }
}

void Model_AttributeDocRef::reinit()
{
  // myLab is unknown, nevertheless, lose of attribute DocRef for live feature seems impossible
}
