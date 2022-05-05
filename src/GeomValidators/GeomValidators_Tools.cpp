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

#include "GeomValidators_Tools.h"

#include "ModelAPI_AttributeRefAttr.h"
#include "ModelAPI_AttributeSelection.h"
#include "ModelAPI_AttributeReference.h"

#include <TopoDS_Iterator.hxx>

namespace GeomValidators_Tools {

  ObjectPtr getObject(const AttributePtr& theAttribute)
  {
    ObjectPtr anObject;
    std::string anAttrType = theAttribute->attributeType();
    if (anAttrType == ModelAPI_AttributeRefAttr::typeId()) {
      AttributeRefAttrPtr anAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
      if (anAttr != NULL && anAttr->isObject())
        anObject = anAttr->object();
    }
    if (anAttrType == ModelAPI_AttributeSelection::typeId()) {
      AttributeSelectionPtr anAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
      if (anAttr != NULL)
        anObject = anAttr->context();
    }
    if (anAttrType == ModelAPI_AttributeReference::typeId()) {
      AttributeReferencePtr anAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
      if (anAttr.get() != NULL)
        anObject = anAttr->value();
    }
    return anObject;
  }

}
