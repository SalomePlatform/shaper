// Copyright (C) 2017-2026  CEA, EDF
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

#ifndef FeaturesAPI_Thickness_H_
#define FeaturesAPI_Thickness_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Thickness.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class FeaturesAPI_Thickness
/// \ingroup CPPHighAPI
/// \brief Interface for Thickness feature.
class FeaturesAPI_Thickness: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Thickness(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Thickness(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const ModelHighAPI_Selection& theBaseObject,
                                 const ModelHighAPI_Double& theThickness,
                                 const bool isInside);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Thickness(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const ModelHighAPI_Selection& theBaseObject,
                                 const ModelHighAPI_Double& theThickness,
                                 const std::list<ModelHighAPI_Selection>& theFaces,
                                 const bool isInside);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Thickness();

  INTERFACE_5(FeaturesPlugin_Thickness::ID(),

              creationMethod, FeaturesPlugin_Thickness::CREATION_METHOD_ID(),
              ModelAPI_AttributeString, /** Creation method */,

              baseObject, FeaturesPlugin_Thickness::BASE_SHAPE_ID(),
              ModelAPI_AttributeSelection, /** Base object */,

              thicknessValue, FeaturesPlugin_Thickness::THICKNESS_VALUE_ID(),
              ModelAPI_AttributeDouble, /** Value of the thickness */,

              faces, FeaturesPlugin_Thickness::FACES_ID(),
              ModelAPI_AttributeSelectionList, /** List of faces to remove in hollowed solid mode */,

              isInside, FeaturesPlugin_Thickness::INSIDE_ID(),
              ModelAPI_AttributeBoolean, /** Do thicken towards inside */)

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on the thickness object.
typedef std::shared_ptr<FeaturesAPI_Thickness> ThicknessPtr;

/// \ingroup CPPHighAPI
/// \brief Create Thickness feature.
FEATURESAPI_EXPORT
ThicknessPtr addThickness(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Selection& theBaseObject,
                          const ModelHighAPI_Double& theThickness,
                          const bool isInside = true);

/// \ingroup CPPHighAPI
/// \brief Create Thickness feature.
FEATURESAPI_EXPORT
ThicknessPtr addHollowedSolid(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Selection& theBaseObject,
                              const ModelHighAPI_Double& theThickness,
                              const std::list<ModelHighAPI_Selection>& theFaces,
                              const bool isInside = true);

#endif // FeaturesAPI_Thickness_H_
