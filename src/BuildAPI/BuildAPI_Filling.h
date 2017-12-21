// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

#ifndef BuildAPI_Filling_H_
#define BuildAPI_Filling_H_

#include "BuildAPI.h"

#include <BuildPlugin_Filling.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Filling
/// \ingroup CPPHighAPI
/// \brief Interface for Filling feature.
class BuildAPI_Filling : public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT explicit BuildAPI_Filling(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with base objects.
  BUILDAPI_EXPORT
  explicit BuildAPI_Filling(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Filling(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const std::string& theOrientCorrection,
                            const int theMinDegree,
                            const int theMaxDegree,
                            const int theNbIter,
                            const double theTolerance2D,
                            const double theTolerance3D,
                            const bool theApproximate);

  /// Destructor.
  BUILDAPI_EXPORT virtual ~BuildAPI_Filling();

  INTERFACE_8(BuildPlugin_Filling::ID(),
              baseObjects, BuildPlugin_Filling::BASE_OBJECTS_ID(),
                           ModelAPI_AttributeSelectionList,
                           /** Base objects */,
              orientationMethod, BuildPlugin_Filling::METHOD_ID(),
                                 ModelAPI_AttributeString,
                                 /** Method to keep edge orientaion */,
              minDegree, BuildPlugin_Filling::MINIMAL_DEGREE_ID(),
                         ModelAPI_AttributeInteger,
                         /** Minimal degree */,
              maxDegree, BuildPlugin_Filling::MAXIMAL_DEGREE_ID(),
                         ModelAPI_AttributeInteger,
                         /** Maximal degree */,
              nbIterations, BuildPlugin_Filling::NUMBER_OF_ITERATIONS_ID(),
                            ModelAPI_AttributeInteger,
                            /** Number of iterations */,
              tolerance2d, BuildPlugin_Filling::TOLERANCE_2D_ID(),
                           ModelAPI_AttributeDouble,
                           /** 2D tolerance */,
              tolerance3d, BuildPlugin_Filling::TOLERANCE_3D_ID(),
                           ModelAPI_AttributeDouble,
                           /** 3D tolerance */,
              approximate, BuildPlugin_Filling::APPROXIMATION_ID(),
                           ModelAPI_AttributeBoolean,
                           /** Approximate original edges */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify orientation method
  BUILDAPI_EXPORT void setOrientationMethod(const std::string& theMethod);

  /// Modify minimal degree of result B-spline
  BUILDAPI_EXPORT void setMinDegree(const int theMinDegree);

  /// Modify maximal degree of result B-spline
  BUILDAPI_EXPORT void setMaxDegree(const int theMaxDegree);

  /// Modify number of iterations
  BUILDAPI_EXPORT void setNbIterations(const int theNbIter);

  /// Set 2D tolerance
  BUILDAPI_EXPORT void setTolerance2d(const double theTol2d);

  /// Set 3D tolerance
  BUILDAPI_EXPORT void setTolerance3d(const double theTol3d);

  /// Set approximation flag
  BUILDAPI_EXPORT void setApproximation(const bool theApproximate);

  /// Dump wrapped feature
  BUILDAPI_EXPORT virtual void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  void execIfBaseNotEmpty();
  void setAdvancedOptions();
};

/// Pointer on Face object.
typedef std::shared_ptr<BuildAPI_Filling> FillingPtr;

/// \ingroup CPPHighAPI
/// \brief Create Filling feature.
BUILDAPI_EXPORT FillingPtr addFilling(const std::shared_ptr<ModelAPI_Document>& thePart,
                                      const std::list<ModelHighAPI_Selection>& theBaseObjects);

/// \ingroup CPPHighAPI
/// \brief Create Filling feature.
BUILDAPI_EXPORT FillingPtr addFilling(
      const std::shared_ptr<ModelAPI_Document>& thePart,
      const std::list<ModelHighAPI_Selection>& theBaseObjects,
      const std::string& theOrientCorrection,
      const int theMinDegree = BuildPlugin_Filling::MINIMAL_DEGREE_DEFAULT(),
      const int theMaxDegree = BuildPlugin_Filling::MAXIMAL_DEGREE_DEFAULT(),
      const int theNbIter = BuildPlugin_Filling::NUMBER_OF_ITERATIONS_DEFAULT(),
      const double theTolerance2D = BuildPlugin_Filling::TOLERANCE_2D_DEFAULT(),
      const double theTolerance3D = BuildPlugin_Filling::TOLERANCE_3D_DEFAULT(),
      const bool theApproximate = BuildPlugin_Filling::APPROXIMATION_DEFAULT());

#endif // BuildAPI_Filling_H_
