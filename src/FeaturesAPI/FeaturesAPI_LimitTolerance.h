// Copyright (C) 2014-2025  CEA, EDF
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

#ifndef FEATURESAPI_LIMITTOLERANCE_H_
#define FEATURESAPI_LIMITTOLERANCE_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_LimitTolerance.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_LimitTolerance
/// \ingroup CPPHighAPI
/// \brief Interface for LimitTolerance feature.
class FeaturesAPI_LimitTolerance: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_LimitTolerance(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_LimitTolerance(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                      const ModelHighAPI_Selection& theShape,
                                      const ModelHighAPI_Double& theTolerance,
                                      const bool theExactCheck);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_LimitTolerance();

  INTERFACE_3(FeaturesPlugin_LimitTolerance::ID(),

              mainObject, FeaturesPlugin_LimitTolerance::OBJECT_ID(),
              ModelAPI_AttributeSelection, /** Main object */,

              tolerance, FeaturesPlugin_LimitTolerance::TOLERANCE_ID(),
              ModelAPI_AttributeDouble, /** Tolerance */,

              exactCheck, FeaturesPlugin_LimitTolerance::EXACT_CHECK_ID(),
              ModelAPI_AttributeBoolean, /** Exact Check Flag */)

  /// Set main object.
  FEATURESAPI_EXPORT
  void setMainObject(const ModelHighAPI_Selection& theMainObject);

  /// Set the tolerance.
  FEATURESAPI_EXPORT
  void setTolerance(const ModelHighAPI_Double& theTolerance);

  /// Set the ExactCheck option.
  FEATURESAPI_EXPORT
  void setExactCheck(const bool theExactCheck);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on LimitTolerance object.
typedef std::shared_ptr<FeaturesAPI_LimitTolerance> LimitTolerancePtr;

/// \ingroup CPPHighAPI
/// \brief Create LimitTolerance feature.
FEATURESAPI_EXPORT
LimitTolerancePtr addLimitTolerance(const std::shared_ptr<ModelAPI_Document>& thePart,
                                    const ModelHighAPI_Selection& theMainObject,
                                    const ModelHighAPI_Double& theTolerance,
                                    const bool theExactCheck = false);

#endif // FEATURESAPI_LIMITTOLERANCE_H_
