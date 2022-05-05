// Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_BoundingBox_H_
#define FeaturesAPI_BoundingBox_H_

#include "FeaturesAPI.h"

#include "FeaturesPlugin_BoundingBox.h"

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

#include <memory>

class ModelAPI_Document;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_NormalToFace
/// \ingroup CPPHighAPI
/// \brief Interface for NormalToface feature.
class FeaturesAPI_BoundingBox: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_BoundingBox(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  FEATURESAPI_EXPORT
  explicit FeaturesAPI_BoundingBox(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const ModelHighAPI_Selection& theObject);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_BoundingBox();

  INTERFACE_1(FeaturesPlugin_BoundingBox::ID(),
              objectSelected, FeaturesPlugin_BoundingBox::OBJECT_ID(),
              ModelAPI_AttributeSelection, /** object selected*/)

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

};

/// Pointer on the NormalToface object.
typedef std::shared_ptr<FeaturesAPI_BoundingBox> BoundingBoxPtr;

/// \ingroup CPPHighAPI
/// \brief get the bounding Box
/// \param thePart the part
/// \param theobject the object selected
FEATURESAPI_EXPORT
BoundingBoxPtr getBoundingBox(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Selection& theObject);

#endif // FeaturesAPI_BoundingBox_H_
