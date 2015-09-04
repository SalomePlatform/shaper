// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_DataMapOfShapeMapOfShapes.h
// Created:     4 September 2015
// Author:      Dmitry Bobylev

#ifndef GeomAPI_DataMapOfShapeMapOfShapes_H_
#define GeomAPI_DataMapOfShapeMapOfShapes_H_

#include <GeomAPI_Interface.h>

#include <GeomAPI_Shape.h>

/**\class GeomAPI_DataMapOfShapeMapOfShapes
 * \ingroup DataModel
 * \brief DataMap of Shape - Map of Shapes defined by TopoDS_Shapes
 */
class GeomAPI_DataMapOfShapeMapOfShapes : public GeomAPI_Interface
{
public:
  /// Constructor.Creates empty map.
  GEOMAPI_EXPORT GeomAPI_DataMapOfShapeMapOfShapes();

  /** \brief Binds list of shapes to the key shape.
      \param[in] theKey key shape.
      \param[in] theItems list of shapes. If shapes have duplications in list only one will be stored.
      \returns true if items bound successfully.
   */
  GEOMAPI_EXPORT bool bind(const std::shared_ptr<GeomAPI_Shape> theKey,
                           const ListOfShape& theItems);

  /** \brief Adds item to the map bounded to the key.
      \param[in] theKey key shape.
      \param[in] theItem item shape.
      \returns true if item bounded successfully. False if it is already bound.
   */
  GEOMAPI_EXPORT bool add(const std::shared_ptr<GeomAPI_Shape> theKey,
                          const std::shared_ptr<GeomAPI_Shape> theItem);

  /// \return true if theKey is stored  in the map.
  GEOMAPI_EXPORT bool isBound(const std::shared_ptr<GeomAPI_Shape> theKey) const;

  /// \return list of shapes bounded to theKey.
  GEOMAPI_EXPORT bool find(const std::shared_ptr<GeomAPI_Shape> theKey,
                           ListOfShape& theItems) const;

  /// Undinds shapes from theKey.
  GEOMAPI_EXPORT bool unBind(const std::shared_ptr<GeomAPI_Shape> theKey);

  /// \return size of map.
  GEOMAPI_EXPORT int size() const;
};

#endif
