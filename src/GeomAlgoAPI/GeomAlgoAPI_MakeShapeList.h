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

#ifndef GeomAlgoAPI_MakeShapeList_H_
#define GeomAlgoAPI_MakeShapeList_H_

#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <memory>

/// \class GeomAlgoAPI_MakeShapeList
/// \ingroup DataAlgo
/// \brief List of topological shapes constructions
class GeomAlgoAPI_MakeShapeList : public GeomAlgoAPI_MakeShape
{
  enum OperationType {
    Generated,
    Modified
  };

public:
  /// Default constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShapeList();

  /// \brief Constructor
  /// \param[in] theMakeShapeList list of algorithms.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShapeList(const ListOfMakeShape& theMakeShapeList);

  /// \brief Initializes a class with new list of algorithms.
  /// \param[in] theMakeShapeList list of algorithms.
  GEOMALGOAPI_EXPORT void init(const ListOfMakeShape& theMakeShapeList);

  /// \brief Adds algo to the end of list.
  /// \param[in] theMakeShape algo to be added.
  GEOMALGOAPI_EXPORT void appendAlgo(const GeomMakeShapePtr theMakeShape);

  /// \return a shape built by the shape construction algorithms
  GEOMALGOAPI_EXPORT virtual const GeomShapePtr shape() const;

  /// \return the list of shapes generated from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void generated(const GeomShapePtr theShape,
                                            ListOfShape& theHistory);

  /// \return the list of shapes modified from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void modified(const GeomShapePtr theShape,
                                           ListOfShape& theHistory);

  /// \return whether the shape is deleted
  GEOMALGOAPI_EXPORT virtual bool isDeleted(const GeomShapePtr theShape);

private:
  void result(const GeomShapePtr theShape,
              OperationType theOperationType,
              ListOfShape& theHistory);

protected:
  ListOfMakeShape myListOfMakeShape; ///< List of make shape algos.
};

#endif
