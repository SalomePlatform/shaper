// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef GeomAPI_DataMapOfShapeMapOfShapes_H_
#define GeomAPI_DataMapOfShapeMapOfShapes_H_

#include "GeomAPI_Interface.h"

#include "GeomAPI_Shape.h"

/// \class GeomAPI_DataMapOfShapeMapOfShapes
/// \ingroup DataModel
/// \brief DataMap of Shape - Map of Shapes defined by TopoDS_Shapes
class GeomAPI_DataMapOfShapeMapOfShapes : public GeomAPI_Interface
{
public:
  /// Constructor.Creates empty map.
  GEOMAPI_EXPORT GeomAPI_DataMapOfShapeMapOfShapes();

  /// \brief Binds list of shapes to the key shape.
  /// \param[in] theKey key shape.
  /// \param[in] theItems list of shapes.
  ///                     If shapes have duplications in list only one will be stored.
  /// \returns true if items bound successfully.
  GEOMAPI_EXPORT bool bind(const std::shared_ptr<GeomAPI_Shape> theKey,
                           const ListOfShape& theItems);

  /// \brief Adds item to the map bounded to the key.
  /// \param[in] theKey key shape.
  /// \param[in] theItem item shape.
  /// \returns true if item bounded successfully. False if it is already bound.
  GEOMAPI_EXPORT bool add(const std::shared_ptr<GeomAPI_Shape> theKey,
                          const std::shared_ptr<GeomAPI_Shape> theItem);

  /// \return true if theKey is stored  in the map.
  GEOMAPI_EXPORT bool isBound(const std::shared_ptr<GeomAPI_Shape> theKey) const;

  /// \return list of shapes bounded to theKey.
  GEOMAPI_EXPORT bool find(const std::shared_ptr<GeomAPI_Shape> theKey,
                           ListOfShape& theItems) const;

  /// Undinds shapes from theKey.
  GEOMAPI_EXPORT bool unBind(const std::shared_ptr<GeomAPI_Shape> theKey);

  /// Clears map.
  GEOMAPI_EXPORT void clear();

  /// \return size of map.
  GEOMAPI_EXPORT int size() const;

public:
  class iterator
  {
  public:
    typedef GeomShapePtr key_type;
    typedef ListOfShape  mapped_type;
    typedef std::pair<GeomShapePtr, ListOfShape> value_type;

  public:
    GEOMAPI_EXPORT iterator();
    GEOMAPI_EXPORT iterator(const iterator&);
    GEOMAPI_EXPORT const iterator& operator=(const iterator&);

    GEOMAPI_EXPORT bool operator==(const iterator&) const;
    GEOMAPI_EXPORT bool operator!=(const iterator&) const;

    GEOMAPI_EXPORT virtual iterator& operator++();
    GEOMAPI_EXPORT virtual iterator  operator++(int);

    GEOMAPI_EXPORT virtual key_type first() const;
    GEOMAPI_EXPORT virtual mapped_type second() const;

  protected:
    std::shared_ptr<iterator> mySelf;
  };

  typedef iterator const_iterator;

  GEOMAPI_EXPORT iterator begin();
  GEOMAPI_EXPORT const_iterator begin() const;

  GEOMAPI_EXPORT iterator end();
  GEOMAPI_EXPORT const_iterator end() const;
};

#endif
