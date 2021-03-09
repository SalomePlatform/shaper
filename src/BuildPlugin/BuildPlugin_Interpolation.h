// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef BuildPlugin_Interpolation_H_
#define BuildPlugin_Interpolation_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

/// \class BuildPlugin_Interpolation
/// \ingroup Plugins
/// \brief Feature for creation of interpolation curve from set of points.
class BuildPlugin_Interpolation : public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  BuildPlugin_Interpolation();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Interpolation");
    return MY_ID;
  }

  /// Attribute name of creation method.
  inline static const std::string& CREATION_METHOD_ID()
  {
    static const std::string MY_CREATION_METHOD_ID("interpolation_method");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name of creation method by selection
  inline static const std::string& CREATION_METHOD_BY_SELECTION_ID()
  {
    static const std::string MY_CREATION_METHOD_BY_SELECTION_ID("by_selection");
    return MY_CREATION_METHOD_BY_SELECTION_ID;
  }

  /// Attribute name of creation method analytical
  inline static const std::string& CREATION_METHOD_ANALYTICAL_ID()
  {
    static const std::string MY_CREATION_METHOD_ANALYTICAL_ID("analytical");
    return MY_CREATION_METHOD_ANALYTICAL_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// Attribute name of closed flag.
  inline static const std::string& CLOSED_ID()
  {
    static const std::string MY_CLOSED_ID("closed");
    return MY_CLOSED_ID;
  }

  /// Attribute name of reorder flag.
  inline static const std::string& REORDER_ID()
  {
    static const std::string MY_REORDER_ID("reorder");
    return MY_REORDER_ID;
  }

  /// Attribute name of use tangents flag.
  inline static const std::string& USE_TANGENTS_ID()
  {
    static const std::string MY_USE_TANGENTS_ID("use_tangents");
    return MY_USE_TANGENTS_ID;
  }

  /// Attribute name of tangent for start point.
  inline static const std::string& TANGENT_START_ID()
  {
    static const std::string MY_TANGENT_START_ID("tangent_start");
    return MY_TANGENT_START_ID;
  }

  /// Attribute name of tangent for end point.
  inline static const std::string& TANGENT_END_ID()
  {
    static const std::string MY_TANGENT_END_ID("tangent_end");
    return MY_TANGENT_END_ID;
  }

  /// Attribute name of x(t) equation.
  inline static const std::string& XT_ID()
  {
    static const std::string MY_XT_ID("xt");
    return MY_XT_ID;
  }

  /// Attribute name of y(t) equation.
  inline static const std::string& YT_ID()
  {
    static const std::string MY_YT_ID("yt");
    return MY_YT_ID;
  }

  /// Attribute name of z(t) equation.
  inline static const std::string& ZT_ID()
  {
    static const std::string MY_ZT_ID("zt");
    return MY_ZT_ID;
  }

  /// Attribute name of min t.
  inline static const std::string& MINT_ID()
  {
    static const std::string MY_MINT_ID("mint");
    return MY_MINT_ID;
  }

  /// Attribute name of max t.
  inline static const std::string& MAXT_ID()
  {
    static const std::string MY_MAXT_ID("maxt");
    return MY_MAXT_ID;
  }

  /// Attribute of parameter name of variable
  inline static const std::string& VARIABLE_ID()
  {
    static const std::string MY_VARIABLE_ID("variable");
    return MY_VARIABLE_ID;
  }

  /// Attribute of parameter name of value
  inline static const std::string& VALUE_ID()
  {
    static const std::string MY_VALUE_ID("value");
    return MY_VALUE_ID;
  }

  /// Attribute of parameter expression error
  inline static const std::string& EXPRESSION_ERROR_ID()
  {
    static const std::string MY_EXPRESSION_ERROR_ID("ExpressionError");
    return MY_EXPRESSION_ERROR_ID;
  }

  /// Attribute name of number of steps
  inline static const std::string& NUMSTEP_ID()
  {
    static const std::string MY_NUMSTEP_ID("numstep");
    return MY_NUMSTEP_ID;
  }

  /// Default value of the closed attribute
  inline static bool CLOSED_DEFAULT() { return false; }

  /// List of references to the arguments of this expression
  inline static const std::string& ARGUMENTS_ID()
  {
    static const std::string MY_ARGUMENTS_ID("arguments");
    return MY_ARGUMENTS_ID;
  }

  /// \return the kind of a feature.
  BUILDPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = BuildPlugin_Interpolation::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  BUILDPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a new part document if needed.
  BUILDPLUGIN_EXPORT virtual void execute();

  /// Called on change of any argument-attribute of this object.
  /// \param[in] theID identifier of changed attribute.
  BUILDPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  protected:
  /// Evaluates the expression x(t), y(t),z(t) in value table.
  void evaluate(std::string& theError);

  /// Update coordinates x,y,z
  void updateCoordinates();

  /// Output error for python interpreter
  std::string outErrorMessage;

};

#endif
