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

#ifndef Model_AttributeRefAttr_H_
#define Model_AttributeRefAttr_H_

#include "Model.h"
#include "ModelAPI_AttributeRefAttr.h"
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDataStd_Comment.hxx>

/**\class Model_AttributeRefAttr
 * \ingroup DataModel
 * \brief Attribute that contains reference to an attribute of a feature 
 * (located in the same document).
 */

class Model_AttributeRefAttr : public ModelAPI_AttributeRefAttr
{
  Handle_TDF_Reference myRef;  ///< reference to the feature label
  TDF_Label myLab; ///< the main label of this attribute
  ///< ID of the referenced attribute (empty if this is a reference to a feature)
  Handle_TDataStd_Comment myID;
 public:
  /// Returns true if this attribute references to a object (not to the attribute)
  MODEL_EXPORT virtual bool isObject();

  /// Defines the reference to the attribute
  MODEL_EXPORT virtual void setAttr(std::shared_ptr<ModelAPI_Attribute> theAttr);

  /// Returns attribute referenced from this attribute
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Attribute> attr();

  /// Defines the reference to the object
  MODEL_EXPORT virtual void setObject(ObjectPtr theFeature);

  /// Returns object referenced from this attribute
  MODEL_EXPORT virtual ObjectPtr object();

  /// Returns true if attribute was  initialized by some value
  MODEL_EXPORT virtual bool isInitialized();

 protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeRefAttr(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
};

#endif
