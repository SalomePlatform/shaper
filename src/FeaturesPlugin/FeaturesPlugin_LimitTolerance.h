// Copyright (C) 2014-2025  CEA, EDF
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

#ifndef FEATURESPLUGIN_LIMITTOLERANCE_H_
#define FEATURESPLUGIN_LIMITTOLERANCE_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>


/** \class FeaturesPlugin_LimitTolerance
 *  \ingroup Plugins
 *  \brief Feature to limit the tolerance on selected shapes.
 */
class FeaturesPlugin_LimitTolerance : public ModelAPI_Feature
{
 public:
  /// LimitTolerance kind.
  inline static const std::string& ID()
  {
    static const std::string MY_LIMIT_TOLERANCE_ID("LimitTolerance");
    return MY_LIMIT_TOLERANCE_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_LimitTolerance::ID();
    return MY_KIND;
  }

  /// Attribute name of referenced object.
  inline static const std::string& OBJECT_ID()
  {
    static const std::string MY_OBJECT_ID("object");
    return MY_OBJECT_ID;
  }

  /// Attribute name for tolerance.
  inline static const std::string& TOLERANCE_ID()
  {
    static const std::string MY_TOLERANCE_ID("tolerance");
    return MY_TOLERANCE_ID;
  }

  /// Attribute name for exact_check.
  inline static const std::string& EXACT_CHECK_ID()
  {
    static const std::string MY_EXACT_CHECK_ID("exact_check");
    return MY_EXACT_CHECK_ID;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Performs the algorithm and stores results in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

public:
  /// Use plugin manager for features creation.
  FeaturesPlugin_LimitTolerance();
};

#endif // FEATURESPLUGIN_LIMITTOLERANCE_H_
