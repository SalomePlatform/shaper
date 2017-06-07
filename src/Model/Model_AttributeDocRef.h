// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Model_AttributeDocRef_H_
#define Model_AttributeDocRef_H_

#include "Model.h"
#include "ModelAPI_AttributeDocRef.h"
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx>

/**\class Model_AttributeDocRef
 * \ingroup DataModel
 * \brief Attribute that contains reference to another document.
 */

class Model_AttributeDocRef : public ModelAPI_AttributeDocRef
{
  /// persistent and unique identifier of the document in the application
  Handle(TDataStd_Integer) myID;
 public:

  /// Defines the document referenced from this attribute
  MODEL_EXPORT virtual void setValue(std::shared_ptr<ModelAPI_Document> theDoc);

  /// Returns document referenced from this attribute
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> value();

  /// Returns the persisten ID of the document
  MODEL_EXPORT virtual int docId();

 protected:
  /// Initializes attibutes
  Model_AttributeDocRef(TDF_Label& theLabel);

  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
};

#endif
