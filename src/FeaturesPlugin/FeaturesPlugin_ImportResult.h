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

#ifndef FeaturesPlugin_ImportResult_H_
#define FeaturesPlugin_ImportResult_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_AttributeValidator.h>

/// \class FeaturesPlugin_ImportResult
/// \ingroup Plugins
/// \brief The Import Result feature allows the user to import one or several results
///        from another Part.

class FeaturesPlugin_ImportResult : public ModelAPI_CompositeFeature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("ImportResult");
    return MY_ID;
  }
  /// All features (list of references)
  inline static const std::string& FEATURES_ID()
  {
    static const std::string MY_FEATURES_ID("Features");
    return MY_FEATURES_ID;
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

  /// Appends a feature
  FEATURESPLUGIN_EXPORT virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID);

  /// \return the number of sub-elements.
  FEATURESPLUGIN_EXPORT virtual int numberOfSubs(bool forTree = false) const;

  /// \return the sub-feature by zero-base index.
  FEATURESPLUGIN_EXPORT virtual
    std::shared_ptr<ModelAPI_Feature> subFeature(const int theIndex, bool forTree = false);

  /// \return the sub-feature unique identifier in this composite feature by zero-base index.
  FEATURESPLUGIN_EXPORT virtual int subFeatureId(const int theIndex) const;

  /// \return true if feature or result belong to this composite feature as subs.
  FEATURESPLUGIN_EXPORT virtual bool isSub(ObjectPtr theObject) const;

  /// This method to inform that sub-feature is removed and must be removed from the internal data
  /// structures of the owner (the remove from the document will be done outside just after).
  FEATURESPLUGIN_EXPORT virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

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
