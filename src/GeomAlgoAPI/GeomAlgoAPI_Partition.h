// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Partition.h
// Created:     21 Aug 2015
// Author:      Sergey POKHODENKO

#ifndef GeomAlgoAPI_Partition_H_
#define GeomAlgoAPI_Partition_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Partition
/// \ingroup DataAlgo
/// \brief Allows to perform of partition operations
class GeomAlgoAPI_Partition : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Creates common partition operation.
  /// \param[in] theObjects the main shape.
  /// \param[in] theTools  second shape.
  /// \return a solid as result of operation.
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> make(const ListOfShape& theObjects,
                                                                const ListOfShape& theTools);

  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Partition(const ListOfShape& theObjects,
                                           const ListOfShape& theTools);

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theObjects,
             const ListOfShape& theTools);
};

#endif
