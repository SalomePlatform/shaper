// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_Pipe_H_
#define FeaturesPlugin_Pipe_H_

#include "FeaturesPlugin.h"

#include <GeomAlgoAPI_Pipe.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>

/// \class FeaturesPlugin_Pipe
/// \ingroup Plugins
/// \brief Feature for creation of extrusion along a path.
/// Pipe creates extrusion of objects along a path.
/// It produces the following results from objects:\n
/// Vertex -> Edge\n
/// Edge -> Face\n
/// Wire -> Shell\n
/// Face -> Solid
class FeaturesPlugin_Pipe : public ModelAPI_Feature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_FEATURE_ID("Pipe");
    return MY_FEATURE_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD("creation_method");
    return MY_CREATION_METHOD;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_SIMPLE()
  {
    static const std::string MY_CREATION_METHOD("simple");
    return MY_CREATION_METHOD;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BINORMAL()
  {
    static const std::string MY_CREATION_METHOD("binormal");
    return MY_CREATION_METHOD;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_LOCATIONS()
  {
    static const std::string MY_CREATION_METHOD("locations");
    return MY_CREATION_METHOD;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// Attribute name of path object.
  inline static const std::string& PATH_OBJECT_ID()
  {
    static const std::string MY_PATH_OBJECT_ID("path_object");
    return MY_PATH_OBJECT_ID;
  }

  /// Attribute name of Bi-Normal.
  inline static const std::string& BINORMAL_ID()
  {
    static const std::string MY_BINORMAL_ID("binormal");
    return MY_BINORMAL_ID;
  }

  /// Attribute name of locations.
  inline static const std::string& LOCATIONS_ID()
  {
    static const std::string MY_LOCATIONS_ID("locations_objects");
    return MY_LOCATIONS_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Pipe::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Pipe();

private:
  void storeResult(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                   const std::shared_ptr<GeomAPI_Shape> thePathShape,
                   const std::shared_ptr<GeomAlgoAPI_Pipe> thePipeAlgo,
                   const int theResultIndex = 0);

  void storeResult(const ListOfShape& theBaseShapes,
                   const std::shared_ptr<GeomAPI_Shape> thePathShape,
                   const std::shared_ptr<GeomAlgoAPI_Pipe> thePipeAlgo,
                   const int theResultIndex = 0);

  void storeShapes(ResultBodyPtr theResultBody,
                   const GeomAPI_Shape::ShapeType theBaseShapeType,
                   const ListOfShape& theShapes,
                   const std::string theName);
};

#endif
