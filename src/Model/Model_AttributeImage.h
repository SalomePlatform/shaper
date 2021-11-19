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

#ifndef MODEL_ATTRIBUTEIMAGE_H_
#define MODEL_ATTRIBUTEIMAGE_H_

#include <Model.h>
#include <ModelAPI_AttributeImage.h>

#include <TDF_Label.hxx>

#include <string>

/**\class Model_AttributeImage
 * \ingroup DataModel
 * \brief API for the attribute that contains image inside.
 * Represented as the following OCCT attributes:
 *   TDataStd_ByteArray for the pixmap texture data,
 *   TDataStd_IntArray(2) for image dimensions (widht and height),
 *   TDataStd_String for image format.
 */

class Model_AttributeImage : public ModelAPI_AttributeImage
{
  /// Stores the label as the only reference to all data
  TDF_Label myLab;

 public:
  /// Defines the value of the image attribute
  MODEL_EXPORT virtual void setTexture(const int theWidth,
                                       const int theHeight,
                                       const std::list<unsigned char>& theByteArray,
                                       const std::string& theFormat,
                                       const bool sendUpdated = true);

  /// Returns true, if texture width and height are non-zero
  MODEL_EXPORT virtual bool hasTexture();

  /// Returns the value of the image attribute
  MODEL_EXPORT virtual bool texture(int& theWidth,
                                    int& theHeight,
                                    std::list<unsigned char>& theByteArray,
                                    std::string& theFormat);

  /// Copy the image data to the destination attribute
  virtual void copyTo(std::shared_ptr<ModelAPI_AttributeImage> theTarget) const;

 protected:
  /// Initializes attibutes
  Model_AttributeImage(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
};

#endif
