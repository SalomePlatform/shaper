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

#ifndef FeaturesPlugin_Placement_H_
#define FeaturesPlugin_Placement_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Placement.h>
#include <GeomAlgoAPI_Transform.h>

class ModelAPI_ResultBody;
class GeomAPI_Shape;

/**\class FeaturesPlugin_Placement
 * \ingroup Plugins
 * \brief Feature for applying of placement operation: relative movement of Solid.
 *
 * Locates the selected placement_attractable_object (face, edge, vertex) of the solid into 
 * the selected placement_base_object. Faces must be planar, edges must be linear.
 * Orientation of the placed solid depends on the underlied planes of both faces.
 */
class FeaturesPlugin_Placement : public ModelAPI_Feature
{
 public:
  /// Placement kind
  inline static const std::string& ID()
  {
    static const std::string MY_PLACEMENT_ID("Placement");
    return MY_PLACEMENT_ID;
  }

  /// attribute name of references sketch entities list, it should contain a sketch result or
  /// a pair a sketch result to sketch face
  inline static const std::string& OBJECTS_LIST_ID()
  {
    static const std::string MY_OBJECTS_LIST_ID("placement_objects_list");
    return MY_OBJECTS_LIST_ID;
  }

  /// attribute name of referenced object
  inline static const std::string& START_SHAPE_ID()
  {
    static const std::string MY_START_SHAPE_ID("placement_start_shape");
    return MY_START_SHAPE_ID;
  }
  /// attribute name of attractable face
  inline static const std::string& END_SHAPE_ID()
  {
    static const std::string MY_END_SHAPE_ID("placement_end_shape");
    return MY_END_SHAPE_ID;
  }
  /// attribute name of flag of reverse direction
  inline static const std::string& REVERSE_ID()
  {
    static const std::string MY_REVERSE_ID("placement_reverse_direction");
    return MY_REVERSE_ID;
  }
  /// attribute name of flag of centering position
  inline static const std::string& CENTERING_ID()
  {
    static const std::string MY_CENTERING_ID("placement_centering");
    return MY_CENTERING_ID;
  }

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Placement::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Placement();
private:
  /// Checks validity of passed shape.
  bool isShapeValid(GeomShapePtr theShape);
};

#endif
