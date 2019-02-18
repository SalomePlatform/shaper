// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <Selector_Primitive.h>

#include <Selector_NameGenerator.h>

#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>


Selector_Primitive::Selector_Primitive() : Selector_Algo()
{
}

void Selector_Primitive::select(const TDF_Label theFinalLabel)
{
  myFinal = theFinalLabel;
}

void Selector_Primitive::store()
{
  storeType(Selector_Algo::SELTYPE_PRIMITIVE);
  static const TDF_LabelList anEmptyRefList;
  storeBaseArray(anEmptyRefList, myFinal);
}

bool Selector_Primitive::restore()
{
  static TDF_LabelList anEmptyRefList;
  return restoreBaseArray(anEmptyRefList, myFinal);
}

TDF_Label Selector_Primitive::restoreByName(std::string theName,
  const TopAbs_ShapeEnum theShapeType, Selector_NameGenerator* theNameGenerator)
{
  TDF_Label aContext;
  if (theNameGenerator->restoreContext(theName, aContext, myFinal)) {
    if (myFinal.IsNull())
      aContext.Nullify();
  }
  return aContext;
}

bool Selector_Primitive::solve(const TopoDS_Shape& theContext)
{
  Handle(TNaming_NamedShape) aNS;
  if (myFinal.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
    TopoDS_Shape aResult = aNS->Get();
    // if shape was modified and not exists in the context anymore, check evolution of this shape
    // issue 2254 and similar (document CEA parametric first issue description)
    findNewVersion(theContext, aResult);
    Selector_Algo::store(aResult);
    return true;
  }
  return false;
}

std::string Selector_Primitive::name(Selector_NameGenerator* theNameGenerator)
{
  Handle(TDataStd_Name) aName;
  if (!myFinal.FindAttribute(TDataStd_Name::GetID(), aName))
    return "";
  std::string aResult = theNameGenerator->contextName(myFinal);
  if (!aResult.empty())
    aResult += "/" + std::string(TCollection_AsciiString(aName->Get()).ToCString());
  return aResult;
}
