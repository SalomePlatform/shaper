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

#ifndef FILTERSAPI_FILTER_H_
#define FILTERSAPI_FILTER_H_

#include "FiltersAPI.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>

class ModelAPI_Attribute;

/**\class FiltersAPI_Filter
 * \ingroup CPPHighAPI
 * \brief Interface for the Filter
 */
class FiltersAPI_Filter
{
public:
  FILTERSAPI_EXPORT
  FiltersAPI_Filter(
      const std::string& theName,
      const bool theRevertFilter = false,
      const std::list<ModelHighAPI_Selection>& theArguments = std::list<ModelHighAPI_Selection>());

  // Internal constructor based on filter arguments
  FILTERSAPI_EXPORT
  FiltersAPI_Filter(const std::string& theName,
                    const std::list<std::shared_ptr<ModelAPI_Attribute> >& theArguments);

  /// Destructor
  FILTERSAPI_EXPORT
  virtual ~FiltersAPI_Filter();

  const std::string& name() const { return myName; }
  bool isReversed() const { return myReversed; }

  /// Dump wrapped feature
  FILTERSAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  std::string myName;
  bool myReversed;
  std::list<ModelHighAPI_Selection> myFilterArguments;
};

typedef std::shared_ptr<FiltersAPI_Filter> FilterAPIPtr;

/// Create list of filters
FILTERSAPI_EXPORT FilterAPIPtr
addFilter(const std::string& name = std::string(),
          const bool exclude = false,
          const std::list<ModelHighAPI_Selection>& args = std::list<ModelHighAPI_Selection>());

#endif
