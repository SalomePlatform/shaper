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

#include "FiltersAPI_Argument.h"

FiltersAPI_Argument::FiltersAPI_Argument()
{
}

FiltersAPI_Argument::FiltersAPI_Argument(const bool theValue)
  : myBoolean(theValue)
{
}

FiltersAPI_Argument::FiltersAPI_Argument(const std::string& theValue)
  : myValue(theValue)
{
}

FiltersAPI_Argument::FiltersAPI_Argument(const ModelHighAPI_Selection& theSelection)
  : mySelection(theSelection)
{
}

FiltersAPI_Argument::FiltersAPI_Argument(const AttributeSelectionPtr& theSelection)
  : mySelectionAttr(theSelection)
{
}

FiltersAPI_Argument::~FiltersAPI_Argument()
{
}

void FiltersAPI_Argument::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (mySelectionAttr) {
    // write empty selection attribute, because parametrization is not supported yet
    theDumper << "model.selection()"; // mySelectionAttr;
  }
  else if (mySelection.variantType() == ModelHighAPI_Selection::VT_Empty) {
    if (myValue.empty())
      theDumper << myBoolean;
    else
      theDumper << "\"" << myValue << "\"";
  }
}
