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

#ifndef Model_AttributeReference_H_
#define Model_AttributeReference_H_

#include "Model.h"
#include "ModelAPI_AttributeReference.h"
#include <ModelAPI_Feature.h>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>

/**\class Model_AttributeReference
 * \ingroup DataModel
 * \brief Attribute that contains reference to feature (located in the same document).
 */

class Model_AttributeReference : public ModelAPI_AttributeReference
{
  TDF_Label myLab; ///< the main label of this attribute
  Handle_TDF_Reference myRef;  ///< references to the feature label
 public:
  /// Defines the object referenced from this attribute
  MODEL_EXPORT virtual void setValue(ObjectPtr theObject);

  /// Returns object referenced from this attribute
  MODEL_EXPORT virtual ObjectPtr value();

  MODEL_EXPORT ~Model_AttributeReference();

  MODEL_EXPORT virtual void setObject(const std::shared_ptr<ModelAPI_Object>& theObject);

  /// Returns true if attribute was  initialized by some value
  MODEL_EXPORT virtual bool isInitialized();


protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeReference(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
  friend class Model_AttributeSelection;
};

#endif
