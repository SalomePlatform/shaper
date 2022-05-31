// Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

// File:        FeaturesPlugin_MultiRotation.h
// Created:     30 Jan 2017
// Author:      Clarisse Genrault (CEA)

#ifndef FEATURESPLUGIN_MULTIROTATION_H_
#define FEATURESPLUGIN_MULTIROTATION_H_

#include <FeaturesPlugin.h>

#include <GeomAlgoAPI_Rotation.h>
#include <GeomAlgoAPI_Translation.h>

#include <ModelAPI_Feature.h>

/** \class FeaturesPlugin_MultiRotation
 *  \ingroup Plugins
 *  \brief Feature that rotaes object around an axis an number of times.
 *         The 2D version also makes translated copies of the object and performs the rotation
 *         on these as well.
 */
class FeaturesPlugin_MultiRotation : public ModelAPI_Feature
{
  public:
  /// Multi-rotation kind.
  inline static const std::string& ID()
  {
    static const std::string MY_MULTIROTATION_ID("AngularCopy");
    return MY_MULTIROTATION_ID;
  }

  /// Attribute name of referenced objects.
  inline static const std::string& OBJECTS_LIST_ID()
  {
    static const std::string MY_OBJECTS_LIST_ID("main_objects");
    return MY_OBJECTS_LIST_ID;
  }

  /// Attribute name of the angular direction.
  inline static const std::string& AXIS_ANGULAR_ID()
  {
    static const std::string MY_AXIS_ANGULAR_ID("axis_angular");
    return MY_AXIS_ANGULAR_ID;
  }

  /// Attribute name for use angular step.
  inline static const std::string& USE_ANGULAR_STEP_ID()
  {
    static const std::string MY_USE_ANGULAR_STEP_ID("use_step_angular");
    return MY_USE_ANGULAR_STEP_ID;
  }

  /// Attribute name of step for the angular.
  inline static const std::string& STEP_ANGULAR_ID()
  {
    static const std::string MY_STEP_ANGULAR_ID("step_angular");
    return MY_STEP_ANGULAR_ID;
  }

  /// Attribute name of number of copies for angular.
  inline static const std::string& NB_COPIES_ANGULAR_ID()
  {
    static const std::string MY_NB_COPIES_ANGULAR_ID("nb_angular");
    return MY_NB_COPIES_ANGULAR_ID;
  }

  /// Attribute name for use radial dir.
  inline static const std::string& USE_RADIAL_DIR_ID()
  {
    static const std::string MY_USE_RADIAL_DIR_ID("use_radial_dir");
    return MY_USE_RADIAL_DIR_ID;
  }

  /// Attribute name of radial step.
  inline static const std::string& STEP_RADIAL_ID()
  {
    static const std::string MY_STEP_RADIAL_ID("step_radial");
    return MY_STEP_RADIAL_ID;
  }

  /// Attribute name of number of copies for radial.
  inline static const std::string& NB_COPIES_RADIAL_ID()
  {
    static const std::string MY_NB_COPIES_RADIAL_ID("nb_radial");
    return MY_NB_COPIES_RADIAL_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_MultiRotation::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_MultiRotation();

private:
  /// Fill axis, number of copies and angle of the rotation.
  /// \returns \c false in case of errors.
  bool paramsOfRotation(std::shared_ptr<GeomAPI_Ax1>& theAxis, double& theAngle, int& theQuantity);

  /// Perform the multi rotation in one direction.
  void performRotation1D();

#ifdef FEATURE_MULTIROTATION_TWO_DIRECTIONS
  /// Perform the multi rotation in two directions.
  void performRotation2D();

  void loadNamingDS2(std::list<std::shared_ptr<GeomAlgoAPI_Translation> > theListOfTranslationAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBaseShape);

  void loadNamingDS3(std::list<std::shared_ptr<GeomAlgoAPI_Rotation> > theListOfRotationAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBaseShape, int nb);
#endif
};

#endif // FEATURESPLUGIN_MULTIROTATION_H_
