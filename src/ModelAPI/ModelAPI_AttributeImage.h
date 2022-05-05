// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef ModelAPI_AttributeImage_H_
#define ModelAPI_AttributeImage_H_

#include <ModelAPI.h>
#include <ModelAPI_Attribute.h>

#include <string>
#include <list>


/**\class ModelAPI_AttributeImage
 * \ingroup DataModel
 * \brief API for the attribute that contains binary data.
 * Used for images storage for an example. By default size is one, zero-based.
 */

class ModelAPI_AttributeImage : public ModelAPI_Attribute
{
 public:
  /// Defines the value of the image attribute
  MODELAPI_EXPORT virtual void setTexture(const int theWidth,
                                          const int theHeight,
                                          const std::list<unsigned char>& theByteArray,
                                          const std::string& theFormat,
                                          const bool sendUpdated = true) = 0;

  /// Returns true, if texture width and height are non-zero
  MODELAPI_EXPORT virtual bool hasTexture() = 0;

  /// Returns the value of the image attribute
  MODELAPI_EXPORT virtual bool texture(int& theWidth,
                                       int& theHeight,
                                       std::list<unsigned char>& theByteArray,
                                       std::string& theFormat) = 0;

  /// Copy the image data to the destination attribute
  virtual void copyTo(std::shared_ptr<ModelAPI_AttributeImage> theTarget) const = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string typeId()
  {
    return "Image";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeImage();

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeImage();
};

//! Pointer on image attribute
typedef std::shared_ptr<ModelAPI_AttributeImage> AttributeImagePtr;

#endif
