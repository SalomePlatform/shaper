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

#ifndef FeaturesPlugin_Revolution_H_
#define FeaturesPlugin_Revolution_H_

#include "FeaturesPlugin.h"

#include "FeaturesPlugin_CompositeSketch.h"

#include <GeomAlgoAPI_MakeShape.h>

/// \class FeaturesPlugin_Revolution
/// \ingroup Plugins
/// \brief Feature for creation of revolution from the planar face.
/// Revolution creates the lateral faces based on edges of the base face and
/// the start and end faces and/or start and end angles.
class FeaturesPlugin_Revolution: public FeaturesPlugin_CompositeSketch
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Revolution");
    return MY_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_THROUGH_ALL()
  {
    static const std::string MY_CREATION_METHOD_THROUGH_ALL("ThroughAll");
    return MY_CREATION_METHOD_THROUGH_ALL;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_ANGLES()
  {
    static const std::string MY_CREATION_METHOD_BY_ANGLES("ByAngles");
    return MY_CREATION_METHOD_BY_ANGLES;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_PLANES()
  {
    static const std::string MY_CREATION_METHOD_BY_PLANES("ByPlanesAndOffsets");
    return MY_CREATION_METHOD_BY_PLANES;
  }

  /// Attribute name of an revolution axis.
  inline static const std::string& AXIS_OBJECT_ID()
  {
    static const std::string MY_AXIS_OBJECT_ID("axis_object");
    return MY_AXIS_OBJECT_ID;
  }

  /// Attribute name of revolution to angle.
  inline static const std::string& TO_ANGLE_ID()
  {
    static const std::string MY_TO_ANGLE_ID("to_angle");
    return MY_TO_ANGLE_ID;
  }

  /// Attribute name of revolution from angle.
  inline static const std::string& FROM_ANGLE_ID()
  {
    static const std::string MY_FROM_ANGLE_ID("from_angle");
    return MY_FROM_ANGLE_ID;
  }

  /// Attribute name of an object to which the revolution grows.
  inline static const std::string& TO_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("to_object");
    return MY_TO_OBJECT_ID;
  }

  /// Attribute name of revolution offset.
  inline static const std::string& TO_OFFSET_ID()
  {
    static const std::string MY_TO_OFFSET_ID("to_offset");
    return MY_TO_OFFSET_ID;
  }

  /// Attribute name of an object from which the revolution grows.
  inline static const std::string& FROM_OBJECT_ID()
  {
    static const std::string MY_FROM_OBJECT_ID("from_object");
    return MY_FROM_OBJECT_ID;
  }

  /// Attribute name of revolution offset.
  inline static const std::string& FROM_OFFSET_ID()
  {
    static const std::string MY_FROM_OFFSET_ID("from_offset");
    return MY_FROM_OFFSET_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Revolution::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Revolution();

  protected:
  /// Generates revolutions.
  /// \param[out] theBaseShapes list of base shapes.
  /// \param[out] theMakeShapes list of according algos.
  /// \return false in case one of algo failed.
  bool makeRevolutions(ListOfShape& theBaseShapes,
                       ListOfMakeShape& theMakeShapes);
};

#endif
