// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_Chamfer_H_
#define FeaturesPlugin_Chamfer_H_

#include "FeaturesPlugin_VersionedChFi.h"

/// \class FeaturesPlugin_Chamfer
/// \ingroup Plugins
/// \brief Feature for applying the Chamfer operations on Solids.
///        Supports two distances chamfer and distance-angle chamfer.
class FeaturesPlugin_Chamfer : public FeaturesPlugin_VersionedChFi
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Chamfer");
    return MY_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Chamfer::ID();
    return MY_KIND;
  }

  inline static const std::string& CREATION_METHOD()
  {
    static std::string MY_CREATION_METHOD("creation_method");
    return MY_CREATION_METHOD;
  }

  inline static const std::string CREATION_METHOD_DISTANCE_DISTANCE()
  {
    static std::string MY_SINGLE_RADIUS("distance_distance");
    return MY_SINGLE_RADIUS;
  }

  inline static const std::string CREATION_METHOD_DISTANCE_ANGLE()
  {
    static std::string MY_VARYING_RADIUS("distance_angle");
    return MY_VARYING_RADIUS;
  }

  /// Attribute name of main objects.
  inline static const std::string& OBJECT_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_objects");
    return MY_OBJECT_LIST_ID;
  }

  /// Attribute name of D1.
  inline static const std::string& D1_ID()
  {
    static const std::string MY_D1_ID("d1");
    return MY_D1_ID;
  }

  /// Attribute name of D2.
  inline static const std::string& D2_ID()
  {
    static const std::string MY_D2_ID("d2");
    return MY_D2_ID;
  }

  /// Attribute name of D.
  inline static const std::string& D_ID()
  {
    static const std::string MY_D_ID("d");
    return MY_D_ID;
  }

  /// Attribute name of Angle.
  inline static const std::string& ANGLE_ID()
  {
    static const std::string MY_ANGLE_ID("angle");
    return MY_ANGLE_ID;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Chamfer();

private:
  /// Return attribute storing the selected objects of the operation.
  virtual std::shared_ptr<ModelAPI_Attribute> objectsAttribute();

  /// Return name of modified shape prefix name
  virtual const std::string& modifiedShapePrefix() const;

  /// Run chamfer/fillet operation and returns the modification algorithm if succeed.
  virtual std::shared_ptr<GeomAlgoAPI_MakeShape>
      performOperation(const GeomShapePtr& theSolid, const ListOfShape& theEdges);
};

#endif
