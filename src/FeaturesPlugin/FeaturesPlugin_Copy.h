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

#ifndef FeaturesPlugin_Copy_H_
#define FeaturesPlugin_Copy_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_Copy
/// \ingroup Plugins
/// \brief This feature copies the selected results and sub-results (for the whole feature selected
///        all results of this feature are copied). The referenced arguments of this feature are
///        not concealed. The difference with ”Recover” feature is that not concealed results may
///        be selected and in the history behavior: the “Move to the End” of groups will move to
///        all copy-results.

class FeaturesPlugin_Copy : public ModelAPI_Feature, public ModelAPI_FeatureCopyInterface
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Copy");
    return MY_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Copy::ID();
    return MY_KIND;
  }

  /// Selection list attribute that contains all copied shapes selection.
  inline static const std::string& OBJECTS()
  {
    static const std::string MY_OBJECTS("objects");
    return MY_OBJECTS;
  }
  /// Integer attribute that contains the number of resulting copies needed
  inline static const std::string NUMBER()
  {
    static std::string MY_NUMBER("number");
    return MY_NUMBER;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// To update the group feature which is moved over this copy feature (to add copies to selection)
  FEATURESPLUGIN_EXPORT virtual void getCopies(
    ObjectPtr theContext, std::shared_ptr<GeomAPI_Shape> theValue,
    std::list<ObjectPtr>& theCopyContext, std::list<std::shared_ptr<GeomAPI_Shape> >& theCopyVals);

  /// Use plugin manager for features creation.
  FeaturesPlugin_Copy() {}
};

#endif
