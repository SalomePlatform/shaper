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

#include <Model_ResultGroup.h>
#include <Model_Data.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAPI_ShapeExplorer.h>

#include <Config_PropManager.h>

#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>

Model_ResultGroup::Model_ResultGroup(std::shared_ptr<ModelAPI_Data> theOwnerData)
{
  myOwnerData = theOwnerData;
}

void Model_ResultGroup::colorConfigInfo(std::string& theSection, std::string& theName,
                                       std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_group_color";
  theDefault = DEFAULT_COLOR();
}

std::shared_ptr<GeomAPI_Shape> Model_ResultGroup::shape()
{
  std::shared_ptr<GeomAPI_Shape> aResult;
  // obtain stored shape
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData && aData->isValid()) {
    TDF_Label aShapeLab = aData->shapeLab();
    Handle(TDF_Reference) aRef;
    if (aShapeLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
      aShapeLab = aRef->Get();
    }
    Handle(TNaming_NamedShape) aName;
    if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
      TopoDS_Shape aShape = aName->Get();
      if (!aShape.IsNull()) {
        aResult.reset(new GeomAPI_Shape);
        aResult->setImpl(new TopoDS_Shape(aShape));
      }
    }
  }
  // collect shapes selected in group
  if (!aResult && myOwnerData) {
    AttributeSelectionListPtr aList = myOwnerData->selectionList("group_list");
    if (aList) {
      GeomAPI_DataMapOfShapeShape aShapesMap; // to avoid shapes duplication
      std::list<std::shared_ptr<GeomAPI_Shape> > aSubs;
      for(int a = aList->size() - 1; a >= 0; a--) {
        std::shared_ptr<GeomAPI_Shape> aSelection = aList->value(a)->value();
        if (aList->isWholeResultAllowed()) { // whole result selection, explode to sub-shapes
          if (!aSelection.get() || aSelection->isNull()) {
            ResultPtr aContext = aList->value(a)->context();
            if (aContext)
              aSelection = aContext->shape();
          }
          if (aSelection && !aSelection->isNull()) {
            GeomAPI_Shape::ShapeType aType = GeomAPI_Shape::shapeTypeByStr(aList->selectionType());
            if (aType == aSelection->shapeType()) {
              if (aShapesMap.bind(aSelection, aSelection))
                aSubs.push_back(aSelection);
            } else {
              for(GeomAPI_ShapeExplorer anExp(aSelection, aType); anExp.more(); anExp.next()) {
                if (aShapesMap.bind(anExp.current(), anExp.current()))
                  aSubs.push_back(anExp.current());
              }
            }
          }
        } else { // take selection as it is
          if (aSelection && !aSelection->isNull()) {
            if (aShapesMap.bind(aSelection, aSelection))
              aSubs.push_back(aSelection);
          }
        }
      }
      if (!aSubs.empty()) {
        aResult = GeomAlgoAPI_CompoundBuilder::compound(aSubs);
      }
    }
  }
  return aResult;
}

void Model_ResultGroup::store(const GeomShapePtr& theShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label aShapeLab = aData->shapeLab();
    aShapeLab.ForgetAttribute(TDF_Reference::GetID());
    aShapeLab.ForgetAttribute(TNaming_NamedShape::GetID());

    if (!theShape)
      return;  // bad shape
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (aShape.IsNull())
      return;  // null shape inside

    // store the new shape as primitive
    TNaming_Builder aBuilder(aShapeLab);
    aBuilder.Generated(aShape);
  }
}
