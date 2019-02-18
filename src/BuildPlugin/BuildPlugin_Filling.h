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

#ifndef BuildPlugin_Filling_H_
#define BuildPlugin_Filling_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

#include <memory>
#include <string>

class GeomAlgoAPI_MakeShape;
class GeomAPI_Edge;
class GeomAPI_Pnt;
class GeomAPI_Shape;

/// \class BuildPlugin_Filling
/// \ingroup Plugins
/// \brief Feature for creation of face from list of edges (1D objects).
class BuildPlugin_Filling: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  BuildPlugin_Filling();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Filling");
    return MY_ID;
  }

  /// \return the kind of a feature.
  BUILDPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = BuildPlugin_Filling::ID();
    return MY_KIND;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// Attribute name of advanced options.
  inline static const std::string& ADVANCED_OPTIONS_ID()
  {
    static const std::string MY_ADVANCED_OPTIONS_ID("advanced_options");
    return MY_ADVANCED_OPTIONS_ID;
  }

  /// Attribute name of method of edge orientation.
  inline static const std::string& METHOD_ID()
  {
    static const std::string MY_METHOD_ID("orientation");
    return MY_METHOD_ID;
  }

  /// Supported methods for edge orientation correction
  struct Method {
    inline static const std::string& AUTO_CORRECT_ORIENTATION()
    {
      static const std::string MY_AUTO_CORRECT_ORIENTATION("auto_correct");
      return MY_AUTO_CORRECT_ORIENTATION;
    }
    inline static const std::string& USE_CURVE_INFORMATION()
    {
      static const std::string MY_USE_CURVE_INFORMATION("curve_info");
      return MY_USE_CURVE_INFORMATION;
    }
    inline static const std::string& USE_EDGES_ORIENTATION()
    {
      static const std::string MY_USE_EDGES_ORIENTATION("edge_orient");
      return MY_USE_EDGES_ORIENTATION;
    }
  };

  /// Attribute name of minimal degree.
  inline static const std::string& MINIMAL_DEGREE_ID()
  {
    static const std::string MY_MINIMAL_DEGREE_ID("min_degree");
    return MY_MINIMAL_DEGREE_ID;
  }

  /// Attribute name of maximal degree.
  inline static const std::string& MAXIMAL_DEGREE_ID()
  {
    static const std::string MY_MAXIMAL_DEGREE_ID("max_degree");
    return MY_MAXIMAL_DEGREE_ID;
  }

  /// Attribute name of number of iterations.
  inline static const std::string& NUMBER_OF_ITERATIONS_ID()
  {
    static const std::string MY_NUMBER_OF_ITERATIONS_ID("nb_iter");
    return MY_NUMBER_OF_ITERATIONS_ID;
  }

  /// Attribute name of 2D tolerance.
  inline static const std::string& TOLERANCE_2D_ID()
  {
    static const std::string MY_TOLERANCE_2D_ID("tol_2d");
    return MY_TOLERANCE_2D_ID;
  }

  /// Attribute name of 3D tolerance.
  inline static const std::string& TOLERANCE_3D_ID()
  {
    static const std::string MY_TOLERANCE_3D_ID("tol_3d");
    return MY_TOLERANCE_3D_ID;
  }

  /// Attribute name of approximation.
  inline static const std::string& APPROXIMATION_ID()
  {
    static const std::string MY_APPROXIMATION_ID("approximation");
    return MY_APPROXIMATION_ID;
  }

  /// Default value of the orientation
  inline static const std::string& METHOD_DEFAULT() { return Method::AUTO_CORRECT_ORIENTATION(); }
  /// Default value of minimal degree
  inline static int MINIMAL_DEGREE_DEFAULT() { return 2; }
  /// Default value of maximal degree
  inline static int MAXIMAL_DEGREE_DEFAULT() { return 5; }
  /// Default value of number of iterations
  inline static int NUMBER_OF_ITERATIONS_DEFAULT() { return 0; }
  /// Default value of 2D tolerance
  inline static double TOLERANCE_2D_DEFAULT() { return 1.e-4; }
  /// Default value of 3D tolerance
  inline static double TOLERANCE_3D_DEFAULT() { return 1.e-4; }
  /// Default value of the approximation attribute
  inline static bool APPROXIMATION_DEFAULT() { return false; }

  /// Request for initialization of data model of the feature: adding all attributes.
  BUILDPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a new part document if needed.
  BUILDPLUGIN_EXPORT virtual void execute();

  /// Called on change of any argument-attribute of this object.
  /// \param[in] theID identifier of changed attribute.
  BUILDPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

private:
  /// Convert shape to edge according to construction method
  std::shared_ptr<GeomAPI_Edge> toEdge(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                       const std::string& theMethod);

  /// Update values of attributes by their default values
  void restoreDefaultParameters();

private:
  std::shared_ptr<GeomAPI_Pnt> myLastEdgeStartPoint;
  std::shared_ptr<GeomAPI_Pnt> myLastEdgeEndPoint;
};

#endif
