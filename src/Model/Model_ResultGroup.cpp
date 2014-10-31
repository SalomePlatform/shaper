// File:        Model_ResultGroup.cpp
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultGroup.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

Model_ResultGroup::Model_ResultGroup(boost::shared_ptr<ModelAPI_Data> theOwnerData)
{
  setIsConcealed(false);
  myOwnerData = theOwnerData;
}

boost::shared_ptr<GeomAPI_Shape> Model_ResultGroup::shape() const
{
  boost::shared_ptr<GeomAPI_Shape> aResult;
  if (myOwnerData) {
    AttributeSelectionListPtr aList = myOwnerData->selectionList("group_list");
    if (aList) {
      std::list<boost::shared_ptr<GeomAPI_Shape> > aSubs;
      for(int a = aList->size(); a >= 0; a--) {
        boost::shared_ptr<GeomAPI_Shape> aSelection = aList->value(a)->value();
        if (aSelection && !aSelection->isNull()) {
          aSubs.push_back(aSelection);
        }
      }
      if (!aSubs.empty()) {
        aResult = GeomAlgoAPI_CompoundBuilder::compound(aSubs);
      }
    }
  }
  return aResult;
}
