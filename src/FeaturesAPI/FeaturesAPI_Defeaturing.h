// Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_Defeaturing_H_
#define FeaturesAPI_Defeaturing_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Defeaturing.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class FeaturesAPI_Defeaturing
/// \ingroup CPPHighAPI
/// \brief Interface for the Defeaturing feature.
class FeaturesAPI_Defeaturing: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Defeaturing(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Defeaturing(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::list<ModelHighAPI_Selection>& theFacesToRemove);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Defeaturing();

  INTERFACE_1(FeaturesPlugin_Defeaturing::ID(),
              baseObjects, FeaturesPlugin_Defeaturing::OBJECT_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify faces to be removed.
  FEATURESAPI_EXPORT
  void setFaces(const std::list<ModelHighAPI_Selection>& theFacesToRemove);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  void execIfBaseNotEmpty();
};

/// Pointer on the Defeaturing object.
typedef std::shared_ptr<FeaturesAPI_Defeaturing> DefeaturingPtr;

/// \ingroup CPPHighAPI
/// \brief Create Defeaturing feature.
FEATURESAPI_EXPORT
DefeaturingPtr addDefeaturing(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const std::list<ModelHighAPI_Selection>& theFaces);

#endif // FeaturesAPI_Defeaturing_H_
