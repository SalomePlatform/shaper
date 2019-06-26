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

void FiltersAPI_Feature::setFilters(const std::list<FilterAPIPtr>& theFilters)
{
  FiltersFeaturePtr aBase = std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(feature());
  for (std::list<FilterAPIPtr>::const_iterator anIt = theFilters.begin();
       anIt != theFilters.end(); ++anIt) {
    aBase->addFilter((*anIt)->name());
    aBase->setReversed((*anIt)->name(), (*anIt)->isReversed());

    const std::list<ModelHighAPI_Selection>& anArgs = (*anIt)->arguments();
    if (!anArgs.empty()) {
      // find selectionList argument and fill it
      std::list<AttributePtr> aFilterArgs = aBase->filterArgs((*anIt)->name());
      for (std::list<AttributePtr>::iterator aFIt = aFilterArgs.begin();
           aFIt != aFilterArgs.end(); ++aFIt) {
        AttributeSelectionListPtr aSelList =
            std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(*aFIt);
        if (aSelList)
          fillAttribute(anArgs, aSelList);
        else {
          AttributeSelectionPtr aSelection =
              std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*aFIt);
          if (aSelection && anArgs.size() == 1)
            fillAttribute(anArgs.front(), aSelection);
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
