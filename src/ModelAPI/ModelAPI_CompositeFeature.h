// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef MODELAPI_COMPOSITEFEATURE_H_
#define MODELAPI_COMPOSITEFEATURE_H_

#include "ModelAPI.h"
#include "ModelAPI_Feature.h"

/**\class ModelAPI_CompositeFeature
 * \ingroup DataModel
 * \brief Feature that consists of other features: like sketcher
 * with edges inside. It just allows to add a feature to this feature only
 * instead of adding in both document and this feature.
 */
class ModelAPI_CompositeFeature : public ModelAPI_Feature
{
public:
  MODELAPI_EXPORT virtual ~ModelAPI_CompositeFeature();

  /// Adds feature to the sketch and to its document
  virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID) = 0;

  /// Returns the number of sub-elements
  virtual int numberOfSubs(bool forTree = false) const = 0;

  /// Returns the sub-feature by zero-base index
  virtual std::shared_ptr<ModelAPI_Feature> subFeature(const int theIndex,
                                                       bool forTree = false) = 0;

  /// Returns the sub-feature unique identifier in this composite feature by zero-base index
  virtual int subFeatureId(const int theIndex) const = 0;

  /// Returns true if feature or result belong to this composite feature as subs
  virtual bool isSub(ObjectPtr theObject) const = 0;

  /// This method to inform that sub-feature is removed and must be removed from the internal data
  /// structures of the owner (the remove from the document will be done outside just after)
  virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature) = 0;

  /// Exchanges IDs of two given features: needed for more correct naming in some cases (issue 769)
  MODELAPI_EXPORT virtual void exchangeIDs(std::shared_ptr<ModelAPI_Feature> theFeature1,
    std::shared_ptr<ModelAPI_Feature> theFeature2);

  /// in addition removes all subs
  MODELAPI_EXPORT virtual void erase();
};

//! Pointer on the composite feature object
typedef std::shared_ptr<ModelAPI_CompositeFeature> CompositeFeaturePtr;

#endif
