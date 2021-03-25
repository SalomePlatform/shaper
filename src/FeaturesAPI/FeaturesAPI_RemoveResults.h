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

#ifndef FeaturesAPI_RemoveResults_H_
#define FeaturesAPI_RemoveResults_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_RemoveResults.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_RemoveResults
/// \ingroup CPPHighAPI
/// \brief Interface for RemoveResults feature.
class FeaturesAPI_RemoveResults: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RemoveResults(const std::shared_ptr<ModelAPI_Feature>& theFeature);
  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RemoveResults(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theResults);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_RemoveResults();

  INTERFACE_1(FeaturesPlugin_RemoveResults::ID(),
              removed, FeaturesPlugin_RemoveResults::RESULTS_ID(),
              ModelAPI_AttributeSelectionList, /** Results to remove */)

  /// Modify removed results arguments.
  FEATURESAPI_EXPORT
  void setRemoved(const std::list<ModelHighAPI_Selection>& theResults);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on RemoveResults object.
typedef std::shared_ptr<FeaturesAPI_RemoveResults> RemoveResultsPtr;

/// \ingroup CPPHighAPI
/// \brief Create RemoveResults feature.
FEATURESAPI_EXPORT
RemoveResultsPtr addRemoveResults(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theResults);

#endif // FeaturesAPI_RemoveResults_H_
