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

#ifndef MODEL_ATTRIBUTESTRING_H_
#define MODEL_ATTRIBUTESTRING_H_

#include <Model.h>
#include <ModelAPI_AttributeString.h>

#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>

#include <string>

/**\class Model_AttributeString
 * \ingroup DataModel
 * \brief Attribute that contains std (null terminated) string.
 */

class Model_AttributeString : public ModelAPI_AttributeString
{
  Handle_TDataStd_Name myString; ///< container of the string value
  TDF_Label myLab; ///< if attribute is not initialized, store label here
 public:
  /// Defines the std::string value
  MODEL_EXPORT virtual void setValue(const std::string& theValue);
  /// Defines the std::wstring value
  MODEL_EXPORT virtual void setValue(const std::wstring& theValue);

  /// Returns the std::string  value
  MODEL_EXPORT virtual std::string value();
  /// Returns the std::wstring  value
  MODEL_EXPORT virtual std::wstring valueW();

 protected:
  /// Initializes attibutes
  Model_AttributeString(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
};

#endif
