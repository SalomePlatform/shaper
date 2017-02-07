// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Tools.h
// Created:     17 November 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Tools_H_
#define FeaturesPlugin_Tools_H_

#include <GeomAlgoAPI_Translation.h>

class ModelAPI_ResultBody;

class FeaturesPlugin_Tools {
public:
  static void storeModifiedShapes(GeomAlgoAPI_MakeShape& theAlgo,
                                  std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                  std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                  const int theFaceTag,
                                  const int theEdgeTag,
                                  const int theVertexTag,
                                  const std::string theName,
                                  GeomAPI_DataMapOfShapeShape& theSubShapes);
};

#endif /* FeaturesPlugin_Tools_H_ */
