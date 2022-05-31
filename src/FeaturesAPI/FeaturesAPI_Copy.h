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

#ifndef FeaturesAPI_Copy_H_
#define FeaturesAPI_Copy_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Copy.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Copy
/// \ingroup CPPHighAPI
/// \brief Interface for Copy feature.
class FeaturesAPI_Copy: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Copy(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Copy(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const int theNumber);

  /// Destructor.
  FEATURESAPI_EXPORT virtual ~FeaturesAPI_Copy();

  INTERFACE_2(FeaturesPlugin_Copy::ID(),
              objects, FeaturesPlugin_Copy::OBJECTS(),
              ModelAPI_AttributeSelectionList, /** Source objects */,
              number, FeaturesPlugin_Copy::NUMBER(),
              ModelAPI_AttributeInteger, /** Number of copies */)

  /// Modify objects attribute of the feature.
  FEATURESAPI_EXPORT void setObjects(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify number of copies attribute of the feature.
  FEATURESAPI_EXPORT void setNumber(const int theNumber);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Copy object.
typedef std::shared_ptr<FeaturesAPI_Copy> CopyPtr;

/// \ingroup CPPHighAPI
/// \brief Create Copy feature.
FEATURESAPI_EXPORT
CopyPtr addCopy(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theObjects,
                const int theNumber);

#endif // FeaturesAPI_Copy_H_
