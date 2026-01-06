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

#ifndef FeaturesAPI_Offset_H_
#define FeaturesAPI_Offset_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Offset.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class FeaturesAPI_Offset
/// \ingroup CPPHighAPI
/// \brief Interface for Offset feature.
class FeaturesAPI_Offset: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Offset(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Offset(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const ModelHighAPI_Selection& theBaseObject,
                              const ModelHighAPI_Double& theOffset,
                              const bool isPipeJoint);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Offset(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const ModelHighAPI_Selection& theBaseObject,
                              const ModelHighAPI_Double& theOffset,
                              const std::list<ModelHighAPI_Selection>& theFaces);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Offset();

  INTERFACE_5(FeaturesPlugin_Offset::ID(),

              creationMethod, FeaturesPlugin_Offset::CREATION_METHOD_ID(),
              ModelAPI_AttributeString, /** Creation method */,

              baseObject, FeaturesPlugin_Offset::BASE_SHAPE_ID(),
              ModelAPI_AttributeSelection, /** Base object */,

              offsetValue, FeaturesPlugin_Offset::OFFSET_VALUE_ID(),
              ModelAPI_AttributeDouble, /** Value of the offset */,

              isPipeJoint, FeaturesPlugin_Offset::PIPE_JOINT_ID(),
              ModelAPI_AttributeBoolean, /** Is pipe or intersection joint */,

              faces, FeaturesPlugin_Offset::FACES_ID(),
              ModelAPI_AttributeSelectionList, /** List of faces for partial offset */)

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on the offset object.
typedef std::shared_ptr<FeaturesAPI_Offset> OffsetPtr;

/// \ingroup CPPHighAPI
/// \brief Create Offset feature.
FEATURESAPI_EXPORT
OffsetPtr addOffset(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theBaseObject,
                    const ModelHighAPI_Double& theOffset,
                    const bool isPipeJoint = true);

/// \ingroup CPPHighAPI
/// \brief Create Offset feature.
FEATURESAPI_EXPORT
OffsetPtr addOffsetPartial(const std::shared_ptr<ModelAPI_Document>& thePart,
                           const ModelHighAPI_Selection& theBaseObject,
                           const ModelHighAPI_Double& theOffset,
                           const std::list<ModelHighAPI_Selection>& theFaces);

#endif // FeaturesAPI_Offset_H_
