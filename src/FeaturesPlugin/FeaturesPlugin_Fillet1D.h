// Copyright (C) 2020  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_Fillet1D_H_
#define FeaturesPlugin_Fillet1D_H_

#include <FeaturesPlugin.h>

#include <GeomAPI_Shape.h>
#include <ModelAPI_Feature.h>

#include <map>

/// \class FeaturesPlugin_Fillet1D
/// \ingroup Plugins
/// \brief Feature for appling fillet on vertices of 3D wire.
class FeaturesPlugin_Fillet1D : public ModelAPI_Feature
{
  typedef std::map<GeomShapePtr, ListOfShape, GeomAPI_Shape::Comparator> MapShapeSubs;

public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Fillet1D");
    return MY_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Fillet1D::ID();
    return MY_KIND;
  }

  inline static const std::string& CREATION_METHOD()
  {
    static std::string MY_CREATION_METHOD("creation_method");
    return MY_CREATION_METHOD;
  }

  inline static const std::string CREATION_BY_WIRES()
  {
    static std::string MY_SINGLE_RADIUS("by_wires");
    return MY_SINGLE_RADIUS;
  }

  inline static const std::string CREATION_BY_VERTICES()
  {
    static std::string MY_VARYING_RADIUS("by_vertices");
    return MY_VARYING_RADIUS;
  }

  /// Attribute name of selected wires.
  inline static const std::string& WIRE_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_wires");
    return MY_OBJECT_LIST_ID;
  }

  /// Attribute name of selected vertices.
  inline static const std::string& VERTEX_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_vertices");
    return MY_OBJECT_LIST_ID;
  }

  /// Attribute name of radius.
  inline static const std::string& RADIUS_ID()
  {
    static const std::string MY_RADIUS_ID("radius");
    return MY_RADIUS_ID;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Performs the fillet algorithm and stores it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Fillet1D();

private:
  /// Get the list of wires and fillet vertices
  /// \retun \c false if errors occured
  bool baseShapes(MapShapeSubs& theWireVertices);

  /// Run fillet operation and store result.
  /// \return \c false if fillet failed.
  bool performFillet(const GeomShapePtr& theWire,
                     const ListOfShape& theVertices,
                     const int theResultIndex);
};

#endif
