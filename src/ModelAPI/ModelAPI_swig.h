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

#ifndef SRC_MODELAPI_MODELAPI_SWIG_H_
#define SRC_MODELAPI_MODELAPI_SWIG_H_

  #include <GeomAPI_swig.h>

  #include "ModelAPI.h"
  #include "ModelAPI_Entity.h"
  #include "ModelAPI_Document.h"
  #include "ModelAPI_Session.h"
  #include "ModelAPI_Object.h"
  #include "ModelAPI_Feature.h"
  #include "ModelAPI_Plugin.h"
  #include "ModelAPI_CompositeFeature.h"
  #include "ModelAPI_Data.h"
  #include "ModelAPI_Attribute.h"
  #include "ModelAPI_AttributeDocRef.h"
  #include "ModelAPI_AttributeDouble.h"
  #include "ModelAPI_AttributeDoubleArray.h"
  #include "ModelAPI_AttributeInteger.h"
  #include "ModelAPI_AttributeIntArray.h"
  #include "ModelAPI_AttributeString.h"
  #include "ModelAPI_AttributeStringArray.h"
  #include "ModelAPI_AttributeReference.h"
  #include "ModelAPI_AttributeRefAttr.h"
  #include "ModelAPI_AttributeSelection.h"
  #include "ModelAPI_AttributeSelectionList.h"
  #include "ModelAPI_AttributeTables.h"
  #include "ModelAPI_AttributeValidator.h"
  #include "ModelAPI_Validator.h"
  #include "ModelAPI_FeatureValidator.h"
  #include "ModelAPI_AttributeRefList.h"
  #include "ModelAPI_AttributeRefAttrList.h"
  #include "ModelAPI_AttributeBoolean.h"
  #include "ModelAPI_Result.h"
  #include "ModelAPI_ResultConstruction.h"
  #include "ModelAPI_ResultBody.h"
  #include "ModelAPI_ResultPart.h"
  #include "ModelAPI_ResultParameter.h"
  #include "ModelAPI_ResultGroup.h"
  #include "ModelAPI_ResultField.h"
  #include "ModelAPI_Tools.h"
  #include "ModelAPI_ResultCompSolid.h"

  #include <memory>
  #include <string>

  template<class T1, class T2>
  std::shared_ptr<T1> shared_ptr_cast(std::shared_ptr<T2> theObject)
  {
    return std::dynamic_pointer_cast<T1>(theObject);
  }

#endif /* SRC_MODELAPI_MODELAPI_SWIG_H_ */
