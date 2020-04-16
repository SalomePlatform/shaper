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

#include <Model_AttributeString.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <TDataStd_UAttribute.hxx>
#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <string>

// on myLabel if the Unicode string was stored
static const Standard_GUID kUVALUE_IDENTIFIER("04cac509-b2fc-4887-b442-d2a86f2fd7bd");

void Model_AttributeString::setValue(const std::string& theValue)
{
  TCollection_ExtendedString aValue(theValue.c_str());
  if (!myIsInitialized || myString->Get() != aValue) {
    if (myString.IsNull())
      myString = TDataStd_Name::Set(myLab, TCollection_ExtendedString());
    myString->Set(aValue);
    myLab.ForgetAttribute(kUVALUE_IDENTIFIER);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void Model_AttributeString::setValue(const std::wstring& theValue)
{
  TCollection_ExtendedString aValue(theValue.c_str());
  if (!myIsInitialized || myString->Get() != aValue) {
    if (myString.IsNull())
      myString = TDataStd_Name::Set(myLab, TCollection_ExtendedString());
    myString->Set(aValue);
    TDataStd_UAttribute::Set(myLab, kUVALUE_IDENTIFIER);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::string Model_AttributeString::value()
{
  if (myString.IsNull())
    return "";  // not initialized
  return TCollection_AsciiString(myString->Get()).ToCString();
}

char16_t* Model_AttributeString::valueU()
{
  if (myString.IsNull()) {   // not initialized
    static TCollection_ExtendedString anEmpty;
    return (char16_t*)(anEmpty.ToExtString());
  }
  return (char16_t*)(myString->Get().ToExtString());
}

bool Model_AttributeString::isUValue() const {
  return !myLab.IsNull() && myLab.IsAttribute(kUVALUE_IDENTIFIER);
}

Model_AttributeString::Model_AttributeString(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_AttributeString::reinit()
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = myLab.FindAttribute(TDataStd_Name::GetID(), myString) == Standard_True;
}
