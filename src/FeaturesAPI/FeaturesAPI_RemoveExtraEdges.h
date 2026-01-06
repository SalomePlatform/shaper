// Copyright (C) 2014-2026  CEA, EDF
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

#ifndef FeaturesAPI_RemoveExtraEdges_H_
#define FeaturesAPI_RemoveExtraEdges_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_RemoveExtraEdges.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_RemoveExtraEdges
/// \ingroup CPPHighAPI
/// \brief Interface for RemoveExtraEdges feature.
class FeaturesAPI_RemoveExtraEdges: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
    explicit FeaturesAPI_RemoveExtraEdges(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
    explicit FeaturesAPI_RemoveExtraEdges(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                          const ModelHighAPI_Selection& theBase,
                                          const bool theDoUniteFaces);

  /// Destructor.
  FEATURESAPI_EXPORT
    virtual ~FeaturesAPI_RemoveExtraEdges();

  INTERFACE_2(FeaturesPlugin_RemoveExtraEdges::ID(),

              base, FeaturesPlugin_RemoveExtraEdges::BASE_SHAPE_ID(),
              ModelAPI_AttributeSelection, /** Base */,

              uniteFaces, FeaturesPlugin_RemoveExtraEdges::UNITE_FACES_ID(),
              ModelAPI_AttributeBoolean, /** Merge faces that lie on common surface */)

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on RemoveExtraEdges object.
typedef std::shared_ptr<FeaturesAPI_RemoveExtraEdges> RemoveExtraEdgesPtr;

/// \ingroup CPPHighAPI
/// \brief Create RemoveExtraEdges feature.
FEATURESAPI_EXPORT
RemoveExtraEdgesPtr addRemoveExtraEdges(const std::shared_ptr<ModelAPI_Document>& thePart,
                                        const ModelHighAPI_Selection& theBase,
                                        const bool theDoUniteFaces = false);

#endif // FeaturesAPI_RemoveExtraEdges_H_
