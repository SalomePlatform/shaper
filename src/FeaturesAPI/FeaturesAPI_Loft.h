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

#ifndef FeaturesAPI_Loft_H_
#define FeaturesAPI_Loft_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Loft.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Loft
/// \ingroup CPPHighAPI
/// \brief Interface for Loft feature.
class FeaturesAPI_Loft: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Loft(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Loft(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const ModelHighAPI_Selection& theFirstObject,
                            const ModelHighAPI_Selection& theSecondObject);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Loft();

  INTERFACE_2(FeaturesPlugin_Loft::ID(),
              fisrstObject, FeaturesPlugin_Loft::FIRST_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** First object */,
              secondObject, FeaturesPlugin_Loft::SECOND_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** second objexct */)

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Loft object.
typedef std::shared_ptr<FeaturesAPI_Loft> LoftPtr;

/// \ingroup CPPHighAPI
/// \brief Create Loft feature.
FEATURESAPI_EXPORT
LoftPtr addLoft(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& theFirstObject,
                const ModelHighAPI_Selection& theSecondObject);

#endif // FeaturesAPI_Loft_H_
