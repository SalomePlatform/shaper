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

#ifndef Model_ResultBody_H_
#define Model_ResultBody_H_

#include "Model.h"
#include <ModelAPI_ResultBody.h>
#include <vector>
#include <map>

/**\class Model_ResultBody
* \ingroup DataModel
* \brief The body (shape) result of a feature.
*
* Provides a shape that may be displayed in the viewer.
* May provide really huge results, so, working with this kind
* of result must be optimized.
* Also provides a conainer of sub-body result in case it is compound or compsolid.
*/
class Model_ResultBody : public ModelAPI_ResultBody
{
  /// Sub-bodies if this is compsolid or compound: zero-based index to subs
  std::vector<ResultBodyPtr> mySubs;
  /// Also keep map of result to index in mySubs to facilitate speed of access from OB
  std::map<ObjectPtr, int> mySubsMap;
  /// Keeps the last state of the concealment flag in order to update it when needed.
  bool myLastConcealed;

public:

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultBody();

  /// load and orient modified shapes for sub-objects
  MODEL_EXPORT virtual void loadAndOrientModifiedShapes (GeomAlgoAPI_MakeShape* theMS,
    std::shared_ptr<GeomAPI_Shape>  theShapeIn, const int  theKindOfShape, const int  theTag,
    const std::string& theName, GeomAPI_DataMapOfShapeShape& theSubShapes,
    const bool theIsStoreSeparate = false, const bool theIsStoreAsGenerated = false,
    const bool theSplitInSubs = false);


  /// Returns the number of sub-elements
  MODEL_EXPORT virtual int numberOfSubs(bool forTree = false) const;

  /// Returns the sub-result by zero-base index
  MODEL_EXPORT virtual ResultBodyPtr subResult(const int theIndex,
    bool forTree = false) const;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  /// Returns theIndex - zero based index of sub if found
  MODEL_EXPORT virtual bool isSub(ObjectPtr theObject, int& theIndex) const;

  /// Returns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// Disables the result body: keeps the resulting shape as selection, but erases the underlaying
  /// naming data structure if theFlag if false. Or restores every  thing on theFlag is true.
  MODEL_EXPORT virtual bool setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
    const bool theFlag);

  /// The compsolid is concealed if at least one of the sub is concealed
  MODEL_EXPORT virtual bool isConcealed();

  /// Sets all subs as concealed in the data tree (referenced by other objects)
  MODEL_EXPORT virtual void setIsConcealed(const bool theValue);

  /// Returns true if the latest modification of this body in the naming history
  // is equal to the given shape
  MODEL_EXPORT virtual bool isLatestEqual(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Returns true is the topology is connected.
  MODEL_EXPORT virtual bool isConnectedTopology();

protected:
  /// Makes a body on the given feature
  Model_ResultBody();

  /// Updates the sub-bodies if shape of this object is composite-solid
  void updateSubs(const std::shared_ptr<GeomAPI_Shape>& theThisShape,
                  const bool theShapeChanged = true);

  // Checks the state of children and partents to send events of creation/erase when needed
  void updateConcealment();

  friend class Model_Objects;
};

#endif
