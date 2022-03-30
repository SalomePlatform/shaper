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

#include <Model_AttributeImage.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_Comment.hxx>
#include <TDF_CopyLabel.hxx>

void Model_AttributeImage::setTexture(const int theWidth,
                                      const int theHeight,
                                      const std::list<unsigned char>& theByteArray,
                                      const std::string& theFormat,
                                      const bool sendUpdated)
{
  if (theWidth > 0 && theHeight > 0 && theByteArray.size() > 0) { // set new data
    // Find or create attributes
    Handle_TDataStd_ByteArray aByteArray =
      TDataStd_ByteArray::Set(myLab, 0, int(theByteArray.size()) - 1);
    Handle_TDataStd_IntegerArray aDimensions =
      TDataStd_IntegerArray::Set(myLab, 0, 1);
    Handle_TDataStd_Comment aFormat = TDataStd_Comment::Set(myLab, theFormat.c_str());

    // Dimensions
    aDimensions->SetValue(0, theWidth);
    aDimensions->SetValue(1, theHeight);

    // Texture
    Handle(TColStd_HArray1OfByte) aNewArray =
      new TColStd_HArray1OfByte(0, int(theByteArray.size()) - 1);
    std::list<unsigned char>::const_iterator itBA = theByteArray.begin();
    for (int j = 0; itBA != theByteArray.end(); ++itBA, ++j) {
      aNewArray->SetValue(j, (Standard_Byte)(*itBA));
    }
    aByteArray->ChangeArray(aNewArray);

    if (sendUpdated)
      owner()->data()->sendAttributeUpdated(this);
  }
  else { // size is zero => arrays must be erased
    bool isForgotten1 = myLab.ForgetAttribute(TDataStd_ByteArray::GetID());
    bool isForgotten2 = myLab.ForgetAttribute(TDataStd_IntegerArray::GetID());
    bool isForgotten3 = myLab.ForgetAttribute(TDataStd_Comment::GetID());

    if (sendUpdated && (isForgotten1 || isForgotten2 || isForgotten3))
      owner()->data()->sendAttributeUpdated(this);
  }
}

bool Model_AttributeImage::hasTexture()
{
  Handle_TDataStd_ByteArray aByteArray;
  Handle_TDataStd_IntegerArray aDimensions;
  if (myLab.FindAttribute(TDataStd_ByteArray::GetID(), aByteArray) == Standard_True &&
      myLab.FindAttribute(TDataStd_IntegerArray::GetID(), aDimensions) == Standard_True) {

    // Dimensions
    if (aDimensions->Value(0) > 0 && aDimensions->Value(1) > 0)
      // Byte array
      return aByteArray->Length() > 0;
  }
  return false;
}

bool Model_AttributeImage::texture(int& theWidth,
                                   int& theHeight,
                                   std::list<unsigned char>& theByteArray,
                                   std::string& theFormat)
{
  // Init return values
  theWidth = 0;
  theHeight = 0;
  theByteArray.clear();
  theFormat = "";

  Handle_TDataStd_ByteArray aByteArray;
  Handle_TDataStd_IntegerArray aDimensions;
  Handle_TDataStd_Comment aFormat;
  if (myLab.FindAttribute(TDataStd_ByteArray::GetID(), aByteArray) == Standard_True &&
      myLab.FindAttribute(TDataStd_IntegerArray::GetID(), aDimensions) == Standard_True &&
      myLab.FindAttribute(TDataStd_Comment::GetID(), aFormat) == Standard_True) {

    // Dimensions
    theWidth  = aDimensions->Value(0);
    theHeight = aDimensions->Value(1);

    // Texture
    const Handle(TColStd_HArray1OfByte) byteArray = aByteArray->InternalArray();
    for (int j = byteArray->Lower(); j <= byteArray->Upper(); j++) {
      theByteArray.push_back((unsigned char)byteArray->Value( j ));
    }

    // Format
    theFormat = TCollection_AsciiString(aFormat->Get()).ToCString();
    return true;
  }

  return false;
}

void Model_AttributeImage::copyTo(AttributeImagePtr theTarget) const
{
  std::shared_ptr<Model_AttributeImage> aTarget =
    std::dynamic_pointer_cast<Model_AttributeImage>(theTarget);
  if (aTarget) {
    //Model_AttributeSelectionList::copyAttrs(myLab, aTarget->myLab);
    TDF_CopyLabel aCopyAlgo (myLab, aTarget->myLab);
    aCopyAlgo.Perform();
    aTarget->reinit();
  }
}

Model_AttributeImage::Model_AttributeImage(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_AttributeImage::reinit()
{
  myIsInitialized = myLab.IsAttribute(TDataStd_ByteArray::GetID());
}
