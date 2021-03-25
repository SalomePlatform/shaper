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

#ifndef FEATURESPLUGIN_MULTITRANSLATION_H_
#define FEATURESPLUGIN_MULTITRANSLATION_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>

class GeomAPI_Dir;

/** \class FeaturesPlugin_MultiTranslation
 *  \ingroup Plugins
 *  \brief Feature for movement objects along one or two axes an arbitary number of times,
 *         making a copy every time.
 */
class FeaturesPlugin_MultiTranslation : public ModelAPI_Feature
{
  public:
  /// Multi-translation kind.
  inline static const std::string& ID()
  {
    static const std::string MY_MULTITRANSLATION_ID("LinearCopy");
    return MY_MULTITRANSLATION_ID;
  }

  /// Attribute name of referenced objects.
  inline static const std::string& OBJECTS_LIST_ID()
  {
    static const std::string MY_OBJECTS_LIST_ID("main_objects");
    return MY_OBJECTS_LIST_ID;
  }

  /// Attribute name of a first direction.
  inline static const std::string& AXIS_FIRST_DIR_ID()
  {
    static const std::string MY_AXIS_FIRST_DIR_ID("axis_first_dir");
    return MY_AXIS_FIRST_DIR_ID;
  }

  /// Attribute name of step for the first direction.
  inline static const std::string& STEP_FIRST_DIR_ID()
  {
    static const std::string MY_STEP_FIRST_DIR_ID("step_first_dir");
    return MY_STEP_FIRST_DIR_ID;
  }

  /// Attribute name of number of copies for the first direction.
  inline static const std::string& NB_COPIES_FIRST_DIR_ID()
  {
    static const std::string MY_NB_COPIES_FIRST_DIR_ID("nb_first_dir");
    return MY_NB_COPIES_FIRST_DIR_ID;
  }

  /// Attribute name for use second dir.
  inline static const std::string& USE_SECOND_DIR_ID()
  {
    static const std::string MY_USE_SECOND_DIR_ID("use_second_dir");
    return MY_USE_SECOND_DIR_ID;
  }

  /// Attribute name of a second direction.
  inline static const std::string& AXIS_SECOND_DIR_ID()
  {
    static const std::string MY_AXIS_SECOND_DIR_ID("axis_second_dir");
    return MY_AXIS_SECOND_DIR_ID;
  }

  /// Attribute name of step for the second direction.
  inline static const std::string& STEP_SECOND_DIR_ID()
  {
    static const std::string MY_STEP_SECOND_DIR_ID("step_second_dir");
    return MY_STEP_SECOND_DIR_ID;
  }

  /// Attribute name of number of copies for the second direction.
  inline static const std::string& NB_COPIES_SECOND_DIR_ID()
  {
    static const std::string MY_NB_COPIES_SECOND_DIR_ID("nb_second_dir");
    return MY_NB_COPIES_SECOND_DIR_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_MultiTranslation::ID();
    return MY_KIND;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_MultiTranslation();

private:
  /// Fill axis, number of copies and distance for the given direction.
  /// \returns \c false in case of errors.
  bool paramsAlongDirection(const int theIndex,
                            std::shared_ptr<GeomAPI_Dir>& theDir,
                            double& theDistance,
                            int& theQuantity);
};

#endif // FEATURESPLUGIN_MULTITRANSLATION_H_