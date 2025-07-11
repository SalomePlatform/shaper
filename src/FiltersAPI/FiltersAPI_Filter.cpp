// Copyright (C) 2014-2025  CEA, EDF
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

#include "FiltersAPI_Filter.h"

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>

#include <ModelHighAPI_Dumper.h>

FiltersAPI_Filter::FiltersAPI_Filter(
    const std::string& theName,
    const bool theRevertFilter,
    const std::list<FiltersAPI_Argument>& theArguments)
  : myName(theName), myReversed(theRevertFilter), myFilterArguments(theArguments)
{
}

FiltersAPI_Filter::FiltersAPI_Filter(const std::string& theName,
                                     const std::list<AttributePtr>& theArguments)
  : myName(theName)
{
  std::list<AttributePtr>::const_iterator anArgIt = theArguments.begin();
  // first attribute is usually for reversing the filter
  AttributeBooleanPtr aBoolAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(*anArgIt);
  if (aBoolAttr) {
    myReversed = aBoolAttr->value();
    ++anArgIt;
  }

  for (; anArgIt != theArguments.end(); ++anArgIt) {
    AttributeSelectionListPtr aSelList =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(*anArgIt);
    if (aSelList) {
      myFilterArguments.push_back(FiltersAPI_Argument(aSelList));
      continue;
    }

    AttributeSelectionPtr aSelection =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*anArgIt);
    if (aSelection) {
      myFilterArguments.push_back(FiltersAPI_Argument(aSelection));
      continue;
    }

    AttributeStringPtr aString = std::dynamic_pointer_cast<ModelAPI_AttributeString>(*anArgIt);
    if (aString) {
      myFilterArguments.push_back(FiltersAPI_Argument(aString->value()));
      continue;
    }

    AttributeDoublePtr aDouble = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(*anArgIt);
    if (aDouble) {
      myFilterArguments.push_back(FiltersAPI_Argument(aDouble->value()));
      continue;
    }

    AttributeBooleanPtr aBoolean = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(*anArgIt);
    if (aBoolean) {
      myFilterArguments.push_back(FiltersAPI_Argument(aBoolean->value()));
      continue;
    }
  }
}

FiltersAPI_Filter::~FiltersAPI_Filter()
{
}

void FiltersAPI_Filter::dump(ModelHighAPI_Dumper& theDumper) const
{
  theDumper << "model.addFilter(name = \"" << myName << "\"";
  if (myReversed)
    theDumper << ", exclude = " << myReversed;
  if (!myFilterArguments.empty()) {
    theDumper << ", args = [";
    bool isFirstArg = true;
    for (std::list<FiltersAPI_Argument>::const_iterator anIt = myFilterArguments.begin();
         anIt != myFilterArguments.end(); ++anIt) {
      if (isFirstArg)
        isFirstArg = false;
      else
        theDumper << ", ";
      anIt->dump(theDumper);
    }
    theDumper << "]";
  }
  theDumper << ")";
}

// ================================================================================================
FilterAPIPtr addFilter(const std::string& name,
                       const bool exclude,
                       const std::list<FiltersAPI_Argument>& args)
{
  return FilterAPIPtr(new FiltersAPI_Filter(name, exclude, args));
}
