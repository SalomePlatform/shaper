// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Boolean.h
// Created:     02 Sept 2014
// Author:      Vitaly Smetannikov

#ifndef GeomAlgoAPI_Boolean_H_
#define GeomAlgoAPI_Boolean_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Boolean
/// \ingroup DataAlgo
/// \brief Allows to perform of boolean operations
class GeomAlgoAPI_Boolean : public GeomAlgoAPI_MakeShape
{
public:
  /// Type of booelan operation
  enum OperationType{
    BOOL_CUT,   ///< Cut objects
    BOOL_FUSE,  ///< Fuse objects
    BOOL_COMMON ///< Take common part of objects
  };

 public:

  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Boolean(const ListOfShape& theObjects,
                                         const ListOfShape& theTools,
                                         const OperationType theOperationType);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theObjects,
             const ListOfShape& theTools,
             const OperationType theOperationType);
};

#endif
