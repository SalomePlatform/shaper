// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef BuildAPI_Edge_H_
#define BuildAPI_Edge_H_

#include "BuildAPI.h"

#include <BuildPlugin_Edge.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Edge
/// \ingroup CPPHighAPI
/// \brief Interface for Edge feature.
class BuildAPI_Edge: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::list<ModelHighAPI_Selection>& theBaseObjects,
                         const bool theComputeIntersections = false);

  /// Constructor by points.
  BUILDAPI_EXPORT
  explicit BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const ModelHighAPI_Selection& theFirstPoint,
                         const ModelHighAPI_Selection& theSecondPoint);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Edge();

  INTERFACE_5(BuildPlugin_Edge::ID(),
              baseObjects, BuildPlugin_Edge::BASE_OBJECTS_ID(),
              ModelAPI_AttributeSelectionList, /** Base objects */,
              creationMethod, BuildPlugin_Edge::CREATION_METHOD(),
              ModelAPI_AttributeString, /** Creation method */,
              firstPoint, BuildPlugin_Edge::FIRST_POINT(),
              ModelAPI_AttributeSelection, /** First point */,
              secondPoint, BuildPlugin_Edge::SECOND_POINT(),
              ModelAPI_AttributeSelection, /** Second point */,
              computeIntersections, BuildPlugin_Edge::INTERSECT_ID(),
              ModelAPI_AttributeBoolean, /** Intersect edges */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Edge object.
typedef std::shared_ptr<BuildAPI_Edge> EdgePtr;

/// \ingroup CPPHighAPI
/// \brief Create Edge feature.
BUILDAPI_EXPORT
EdgePtr addEdge(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const bool theComputeIntersection = false);
/// \ingroup CPPHighAPI
/// \brief Create Edge feature.
BUILDAPI_EXPORT
EdgePtr addEdge(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& theFirstPoint,
                const ModelHighAPI_Selection& theSecondPoint);

#endif // BuildAPI_Edge_H_
