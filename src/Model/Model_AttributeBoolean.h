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

#ifndef Model_AttributeBoolean_H_
#define Model_AttributeBoolean_H_

#include "Model.h"
#include "ModelAPI_AttributeBoolean.h"
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx>

/**\class Model_AttributeBoolean
 * \ingroup DataModel
 * \brief Attribute that contains real value with double precision.
 */

class Model_AttributeBoolean : public ModelAPI_AttributeBoolean
{
  Handle_TDataStd_Integer myBool;  ///< double is Real attribute
  TDF_Label myLab; ///< if attribute is not initialized, store label here
 public:
  /// Defines the double value
  MODEL_EXPORT virtual void setValue(bool theValue);

  /// Returns the double value
  MODEL_EXPORT virtual bool value();

 protected:
  /// Initializes attibutes
  Model_AttributeBoolean(TDF_Label& theLabel);

  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
};

#endif
