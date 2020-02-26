// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_Chamfer_H_
#define FeaturesAPI_Chamfer_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Chamfer.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Chamfer
/// \ingroup CPPHighAPI
/// \brief Interface for Chamfer feature.
class FeaturesAPI_Chamfer: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Chamfer(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Chamfer(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                               const std::list<ModelHighAPI_Selection>& theBaseObjects,
                               const bool performDistances,
                               const ModelHighAPI_Double& aVal1,
                               const ModelHighAPI_Double& aVal2);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Chamfer();

  INTERFACE_6(FeaturesPlugin_Chamfer::ID(),
              creationMethod, FeaturesPlugin_Chamfer::CREATION_METHOD(),
                              ModelAPI_AttributeString,
                              /** Creation method */,
              baseObjects, FeaturesPlugin_Chamfer::OBJECT_LIST_ID(),
                           ModelAPI_AttributeSelectionList,
                           /** Base objects */,
              d1, FeaturesPlugin_Chamfer::D1_ID(),
                  ModelAPI_AttributeDouble,
                  /** Value of the first distance chamfer */,
              d2, FeaturesPlugin_Chamfer::D2_ID(),
                  ModelAPI_AttributeDouble,
                  /** Value of the second distance chamfer */,
              d, FeaturesPlugin_Chamfer::D_ID(),
                 ModelAPI_AttributeDouble,
                 /** Value of the distance chamfer */,
              angle, FeaturesPlugin_Chamfer::ANGLE_ID(),
                     ModelAPI_AttributeDouble,
                     /** Angle chamfer */)

  /// Modify base objects of the chamfer.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify chamfer to have two distances
  FEATURESAPI_EXPORT
  void setDistances(const ModelHighAPI_Double& theD1, const ModelHighAPI_Double& theD2);

  /// Modify chamfer to have distance  and angle
  FEATURESAPI_EXPORT
  void setDistAngle(const ModelHighAPI_Double& theDistance, const ModelHighAPI_Double& theAngle);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  void execIfBaseNotEmpty();
};

/// Pointer on Chamfer object.
typedef std::shared_ptr<FeaturesAPI_Chamfer> ChamferPtr;

/// \ingroup CPPHighAPI
/// \brief Create Chamfer feature.
FEATURESAPI_EXPORT
ChamferPtr addChamfer(const std::shared_ptr<ModelAPI_Document>& thePart,
                      const std::list<ModelHighAPI_Selection>& theBaseObjects,
                      const bool performDistances,
                      const ModelHighAPI_Double& theVal1,
                      const ModelHighAPI_Double& theVal2,
                      const bool keepSubResults = false);

#endif // FeaturesAPI_Chamfer_H_
