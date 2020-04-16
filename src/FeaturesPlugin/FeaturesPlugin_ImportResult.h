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

#ifndef FeaturesPlugin_ImportResult_H_
#define FeaturesPlugin_ImportResult_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_AttributeValidator.h>

/// \class FeaturesPlugin_ImportResult
/// \ingroup Plugins
/// \brief The Import Result feature allows the user to import one or several results
///        from another Part.

class FeaturesPlugin_ImportResult : public ModelAPI_Feature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("ImportResult");
    return MY_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_ImportResult::ID();
    return MY_KIND;
  }

  /// Selection list attribute that contains all copied shapes selection.
  inline static const std::string& OBJECTS()
  {
    static const std::string MY_OBJECTS("objects");
    return MY_OBJECTS;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_ImportResult() {}
};

/// \class FeaturesPlugin_ValidatorImportResults
/// \ingroup Validators
/// \brief A validator for selection of objects that may be imported:
///        from another part, result bodies.
class FeaturesPlugin_ValidatorImportResults : public ModelAPI_AttributeValidator
{
public:
  //! \return True if selection is valid.
  //! \param[in] theAttribute the checked attribute.
  //! \param[in] theArguments arguments of the attribute.
  //! \param[out] theError error message.
  virtual bool isValid(const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments, Events_InfoMessage& theError) const;
};

#endif
