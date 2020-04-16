// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

// File:        PrimitivesPlugin_Sphere.h
// Created:     15 Mar 2017
// Author:      Clarisse Genrault (CEA)

#ifndef PRIMITIVESPLUGIN_SPHERE_H_
#define PRIMITIVESPLUGIN_SPHERE_H_

#include <PrimitivesPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Sphere.h>

/**\class PrimitivesPlugin_Sphere
 * \ingroup Plugins
 * \brief Feature for creation of a sphere.
 *
 * Creates a sphere from a radius and a center point defaulting to the origin
 */
class PrimitivesPlugin_Sphere : public ModelAPI_Feature
{
 public:
  /// Sphere kind
  inline static const std::string& ID()
  {
    static const std::string MY_SPHERE_ID("Sphere");
    return MY_SPHERE_ID;
  }

  /// Attribute name of the base point
  inline static const std::string& CENTER_POINT_ID()
  {
    static const std::string MY_CENTER_POINT_ID("center_point");
    return MY_CENTER_POINT_ID;
  }

  /// Attribute name of the radius
  inline static const std::string& RADIUS_ID()
  {
    static const std::string MY_RADIUS_ID("radius");
    return MY_RADIUS_ID;
  }

  /// Returns the kind of a feature
  PRIMITIVESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PrimitivesPlugin_Sphere::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  PRIMITIVESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PRIMITIVESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PrimitivesPlugin_Sphere();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Sphere> theSphereAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultSphere);

};

#endif // PRIMITIVESPLUGIN_SPHERE_H_
