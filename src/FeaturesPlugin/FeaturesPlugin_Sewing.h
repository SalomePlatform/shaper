// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef FEATURESPLUGIN_SEWING_H_
#define FEATURESPLUGIN_SEWING_H_

#include <FeaturesPlugin.h>

#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>


/** \class FeaturesPlugin_Sewing
 *  \ingroup Plugins
 *  \brief Feature to perform sewing operation on objects.
 */
class FeaturesPlugin_Sewing : public ModelAPI_Feature
{
 public:
  /// Sewing kind.
  inline static const std::string& ID()
  {
    static const std::string MY_SEWING_ID("Sewing");
    return MY_SEWING_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Sewing::ID();
    return MY_KIND;
  }

  /// Attribute name of referenced objects.
  inline static const std::string& OBJECTS_LIST_ID()
  {
    static const std::string MY_OBJECTS_LIST_ID("main_objects");
    return MY_OBJECTS_LIST_ID;
  }

  /// Attribute name for non-manifold state.
  inline static const std::string& ALLOW_NON_MANIFOLD_ID()
  {
    static const std::string MY_ALLOW_NON_MANIFOLD_ID("allow_non_manifold");
    return MY_ALLOW_NON_MANIFOLD_ID;
  }

  /// Attribute name for tolerance.
  inline static const std::string& TOLERANCE_ID()
  {
    static const std::string MY_TOLERANCE_ID("tolerance");
    return MY_TOLERANCE_ID;
  }

  /// Attribute name for result creation state.
  inline static const std::string& ALWAYS_CREATE_RESULT_ID()
  {
    static const std::string MY_ALWAYS_CREATE_RESULT_ID("always_create_result");
    return MY_ALWAYS_CREATE_RESULT_ID;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Performs the algorithm and stores results in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

public:
  /// Use plugin manager for features creation.
  FeaturesPlugin_Sewing();

private:
  /// Retrieve all shapes from the selection list
  void getOriginalShapes(const std::string& theAttributeName,
                         ListOfShape&       theShapes);

  /// Check, whether the result is sewn or not
  bool isSewn(const ListOfShape& theInputs,
              const GeomShapePtr theResult);
};

#endif // FEATURESPLUGIN_SEWING_H_
