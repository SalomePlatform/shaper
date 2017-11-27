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

#ifndef FeaturesAPI_RemoveSubShapes_H_
#define FeaturesAPI_RemoveSubShapes_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_RemoveSubShapes.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_RemoveSubShapes
/// \ingroup CPPHighAPI
/// \brief Interface for RemoveSubShapes feature.
class FeaturesAPI_RemoveSubShapes: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RemoveSubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RemoveSubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theBase);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_RemoveSubShapes();

  INTERFACE_4(FeaturesPlugin_RemoveSubShapes::ID(),
              base, FeaturesPlugin_RemoveSubShapes::BASE_SHAPE_ID(),
              ModelAPI_AttributeSelection, /** Base */,
              creationMethod, FeaturesPlugin_RemoveSubShapes::CREATION_METHOD(),
              ModelAPI_AttributeString, /** Creation method */,
              subshapesToKeep, FeaturesPlugin_RemoveSubShapes::SUBSHAPES_TO_KEEP_ID(),
              ModelAPI_AttributeSelectionList, /** Subshapes to keep*/,
              subshapesToRemove, FeaturesPlugin_RemoveSubShapes::SUBSHAPES_TO_REMOVE_ID(),
              ModelAPI_AttributeSelectionList, /** Subshapes to remove*/)

  /// Modify objects attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const ModelHighAPI_Selection& theBase);

  /// Modify tools attribute of the feature.
  FEATURESAPI_EXPORT
  void setSubShapesToKeep(const std::list<ModelHighAPI_Selection>& theSubShapes);

  /// Modify tools attribute of the feature.
  FEATURESAPI_EXPORT
  void setSubShapesToRemove(const std::list<ModelHighAPI_Selection>& theSubShapes);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on RemoveSubShapes object.
typedef std::shared_ptr<FeaturesAPI_RemoveSubShapes> RemoveSubShapesPtr;

/// \ingroup CPPHighAPI
/// \brief Create RemoveSubShapes feature.
FEATURESAPI_EXPORT
RemoveSubShapesPtr addRemoveSubShapes(const std::shared_ptr<ModelAPI_Document>& thePart,
                                      const ModelHighAPI_Selection& theBase);

#endif // FeaturesAPI_RemoveSubShapes_H_
