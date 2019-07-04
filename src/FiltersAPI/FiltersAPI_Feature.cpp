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

#include "FiltersAPI_Feature.h"

#include <ModelAPI_Feature.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

FiltersAPI_Feature::FiltersAPI_Feature(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

FiltersAPI_Feature::~FiltersAPI_Feature()
{
}

static void separateArguments(const std::list<FiltersAPI_Argument>& theArguments,
                              std::list<ModelHighAPI_Selection>& theSelections,
                              std::list<std::string>& theTextArgs)
{
  std::list<FiltersAPI_Argument>::const_iterator anIt = theArguments.begin();
  for (; anIt != theArguments.end(); ++anIt) {
    if (anIt->selection().variantType() != ModelHighAPI_Selection::VT_Empty)
      theSelections.push_back(anIt->selection());
    else if (!anIt->string().empty())
      theTextArgs.push_back(anIt->string());
  }
}

void FiltersAPI_Feature::setFilters(const std::list<FilterAPIPtr>& theFilters)
{
  FiltersFeaturePtr aBase = std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(feature());
  for (std::list<FilterAPIPtr>::const_iterator anIt = theFilters.begin();
       anIt != theFilters.end(); ++anIt) {
    aBase->addFilter((*anIt)->name());
    aBase->setReversed((*anIt)->name(), (*anIt)->isReversed());

    const std::list<FiltersAPI_Argument>& anArgs = (*anIt)->arguments();
    if (!anArgs.empty()) {
      // separate selection arguments and strings
      std::list<ModelHighAPI_Selection> aSelections;
      std::list<std::string> aTexts;
      separateArguments(anArgs, aSelections, aTexts);

      // fill arguments of the filter
      std::list<AttributePtr> aFilterArgs = aBase->filterArgs((*anIt)->name());
      for (std::list<AttributePtr>::iterator aFIt = aFilterArgs.begin();
           aFIt != aFilterArgs.end(); ++aFIt) {
        AttributeSelectionListPtr aSelList =
            std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(*aFIt);
        if (aSelList)
          fillAttribute(aSelections, aSelList);
        else {
          AttributeSelectionPtr aSelection =
              std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*aFIt);
          if (aSelection && aSelections.size() == 1)
            fillAttribute(aSelections.front(), aSelection);
          else {
            AttributeStringPtr aString =
                std::dynamic_pointer_cast<ModelAPI_AttributeString>(*aFIt);
            if (aString && aTexts.size() == 1)
              fillAttribute(aTexts.front(), aString);
          }
        }
      }
    }
  }
}

void FiltersAPI_Feature::dump(ModelHighAPI_Dumper& theDumper) const
{
  FiltersFeaturePtr aBase = std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(feature());
  if (!aBase)
    return;

  const std::string& aDocName = theDumper.name(aBase->document());
  theDumper << "model.filters(" << aDocName << ", [";

  std::list<std::string> aFilters = aBase->filters();
  for (std::list<std::string>::iterator aFIt = aFilters.begin(); aFIt != aFilters.end(); ++aFIt) {
    FiltersAPI_Filter aFilter(*aFIt, aBase->filterArgs(*aFIt));
    if (aFIt != aFilters.begin())
      theDumper << ", ";
    aFilter.dump(theDumper);
  }

  theDumper << "])";
}
