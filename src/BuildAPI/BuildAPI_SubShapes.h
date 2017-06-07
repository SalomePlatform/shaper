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

#ifndef BuildAPI_SubShapes_H_
#define BuildAPI_SubShapes_H_

#include "BuildAPI.h"

#include <BuildPlugin_SubShapes.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_SubShapes
/// \ingroup CPPHighAPI
/// \brief Interface for SubShapes feature.
class BuildAPI_SubShapes: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_SubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_SubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const ModelHighAPI_Selection& theBaseShape,
                              const std::list<ModelHighAPI_Selection>& theSubShapes);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_SubShapes();

  INTERFACE_2(BuildPlugin_SubShapes::ID(),
              baseShape, BuildPlugin_SubShapes::BASE_SHAPE_ID(),
              ModelAPI_AttributeSelection, /** Base shape */,
              subShapes, BuildPlugin_SubShapes::SUBSHAPES_ID(),
              ModelAPI_AttributeSelectionList, /** Sub-shapes */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBaseShape(const ModelHighAPI_Selection& theBaseShape);

  /// Modify sub-shapes attribute of the feature.
  BUILDAPI_EXPORT
  void setSubShapes(const std::list<ModelHighAPI_Selection>& theSubShapes);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on SubShapes object.
typedef std::shared_ptr<BuildAPI_SubShapes> SubShapesPtr;

/// \ingroup CPPHighAPI
/// \brief Create SubShapes feature.
BUILDAPI_EXPORT
SubShapesPtr addSubShapes(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const ModelHighAPI_Selection& theBaseShape,
                          const std::list<ModelHighAPI_Selection>& theSubShapes);

#endif // BuildAPI_SubShapes_H_
