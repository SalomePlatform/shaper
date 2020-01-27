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

#ifndef FeaturesAPI_ImportResult_H_
#define FeaturesAPI_ImportResult_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_ImportResult.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_ImportResult
/// \ingroup CPPHighAPI
/// \brief Interface for ImportResult feature.
class FeaturesAPI_ImportResult: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ImportResult(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_ImportResult(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  FEATURESAPI_EXPORT virtual ~FeaturesAPI_ImportResult();

  INTERFACE_1(FeaturesPlugin_ImportResult::ID(),
              objects, FeaturesPlugin_ImportResult::OBJECTS(),
              ModelAPI_AttributeSelectionList, /** Source objects */)

  /// Modify objects attribute of the feature.
  FEATURESAPI_EXPORT void setObjects(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on ImportResult object.
typedef std::shared_ptr<FeaturesAPI_ImportResult> ImportResultPtr;

/// \ingroup CPPHighAPI
/// \brief Create ImportResult feature.
FEATURESAPI_EXPORT
ImportResultPtr addImportResult(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theObjects);

#endif // FeaturesAPI_ImportResult_H_
