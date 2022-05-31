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

#ifndef BuildAPI_Interpolation_H_
#define BuildAPI_Interpolation_H_

#include "BuildAPI.h"

#include <BuildPlugin_Interpolation.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Selection.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Interpolation
/// \ingroup CPPHighAPI
/// \brief Interface for Interpolation feature.
class BuildAPI_Interpolation : public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
    explicit BuildAPI_Interpolation(const FeaturePtr& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
    explicit BuildAPI_Interpolation(const FeaturePtr& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const ModelHighAPI_Selection& theStartTangent,
                                    const ModelHighAPI_Selection& theEndTangent,
                                    const bool theIsClosed, const bool theIsToReorder);

  /// Constructor with base objects, closed and reorder parameters.
  BUILDAPI_EXPORT
    explicit BuildAPI_Interpolation(const FeaturePtr& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                    const bool theIsClosed, const bool theIsToReorder);

  /// Constructor with expression analytical of X,Y andZ
  BUILDAPI_EXPORT
    explicit BuildAPI_Interpolation(const FeaturePtr& theFeature,
                                    const std::string & theXTexpression,
                                    const std::string & theYTexpression,
                                    const std::string & theZTexpression,
                                    const ModelHighAPI_Double& theMinT,
                                    const ModelHighAPI_Double& theMaxT,
                                    const ModelHighAPI_Integer& theNbStep);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Interpolation();

  INTERFACE_13(BuildPlugin_Interpolation::ID(),
              baseObjects, BuildPlugin_Interpolation::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */,
              closed, BuildPlugin_Interpolation::CLOSED_ID(),
              ModelAPI_AttributeBoolean, /** Closed flag */,
              reorder, BuildPlugin_Interpolation::REORDER_ID(),
              ModelAPI_AttributeBoolean, /** Reorder flag */,
              useTangents, BuildPlugin_Interpolation::USE_TANGENTS_ID(),
              ModelAPI_AttributeString, /** Use tangents flag */,
              startTangent, BuildPlugin_Interpolation::TANGENT_START_ID(),
              ModelAPI_AttributeSelection, /** Start point tangent */,
              endTangent, BuildPlugin_Interpolation::TANGENT_END_ID(),
              ModelAPI_AttributeSelection, /** End point tangent */,
              xt, BuildPlugin_Interpolation::XT_ID(),
              ModelAPI_AttributeString, /** xt expression*/,
              yt, BuildPlugin_Interpolation::YT_ID(),
              ModelAPI_AttributeString, /** yt expression*/,
              zt, BuildPlugin_Interpolation::ZT_ID(),
              ModelAPI_AttributeString, /** zt expression*/,
              mint, BuildPlugin_Interpolation::MINT_ID(),
              ModelAPI_AttributeDouble, /** Min*/,
              maxt, BuildPlugin_Interpolation::MAXT_ID(),
              ModelAPI_AttributeDouble, /** Max*/,
              numstep, BuildPlugin_Interpolation::NUMSTEP_ID(),
              ModelAPI_AttributeInteger, /** Number of steps*/,
              creationmethod, BuildPlugin_Interpolation::CREATION_METHOD_ID(),
              ModelAPI_AttributeString, /** Creation method*/)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Set closed flag
  BUILDAPI_EXPORT void setClosed(const bool theIsClosed);

  /// Set reorder flag
  BUILDAPI_EXPORT void setReorder(const bool theIsToReorder);

  /// Set use tangents flag
  BUILDAPI_EXPORT void setUseTangents(const bool theIsToUseTangents);

  /// Set start and end tangents
  BUILDAPI_EXPORT void setTangents(const ModelHighAPI_Selection& theStartTangent,
                                   const ModelHighAPI_Selection& theEndTangent);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  void execIfBaseNotEmpty();
};

/// Pointer on Interpolation object.
typedef std::shared_ptr<BuildAPI_Interpolation> InterpolationPtr;

/// \ingroup CPPHighAPI
/// \brief Create Interpolation feature.
BUILDAPI_EXPORT
InterpolationPtr addInterpolation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const bool theIsClosed = false,
                                  const bool theIsToReorder = false);

/// \ingroup CPPHighAPI
/// \brief Create Interpolation feature using tangents.
BUILDAPI_EXPORT
InterpolationPtr addInterpolation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theStartTangent,
                                  const ModelHighAPI_Selection& theEndTangent,
                                  const bool theIsClosed = false,
                                  const bool theIsToReorder = false);

/// \ingroup CPPHighAPI
/// \brief Create Interpolation feature using tangents.
BUILDAPI_EXPORT
InterpolationPtr addInterpolation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::string & theXTexpression,
                                  const std::string & theYTexpression,
                                  const std::string & theZTexpression,
                                  const ModelHighAPI_Double& theMinT,
                                  const ModelHighAPI_Double& theMaxT,
                                  const ModelHighAPI_Integer& theNbStep);

#endif // BuildAPI_Interpolation_H_
