// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.cpp
// Created:     20 Jul 2016
// Author:      Natalia ERMOLAEVA

#include "ModelGeomAlgo_Point2D.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Dir.h>

namespace ModelGeomAlgo_Point2D {
  std::shared_ptr<GeomDataAPI_Point2D> getPointOfRefAttr(ModelAPI_Feature* theFeature,
                                                         const std::string& theAttribute,
                                                         const std::string& theObjectFeatureKind,
                                                         const std::string& theObjectFeatureAttribute)
  {
    std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

    /// essential check as it is called in openGl thread
    if (!theFeature || !theFeature->data().get() || !theFeature->data()->isValid())
      return std::shared_ptr<GeomDataAPI_Point2D>();

    FeaturePtr aFeature;
    std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeRefAttr>(theFeature->data()->attribute(theAttribute));
    if(!anAttr.get()) {
      return std::shared_ptr<GeomDataAPI_Point2D>();
    }
    aFeature = ModelAPI_Feature::feature(anAttr->object());

    bool aFeatureOfObjectKind = !theObjectFeatureKind.empty() &&
                                !theObjectFeatureAttribute.empty() &&
                                aFeature->getKind() == theObjectFeatureKind;
    if (aFeature.get() && aFeatureOfObjectKind)
        aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                aFeature->data()->attribute(theObjectFeatureAttribute));
    else if (anAttr->attr())
      aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());

    return aPointAttr;
  }

  void getPointsOfReference(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const std::string& theReferenceFeatureKind,
                            std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                            const std::string& theObjectFeatureKind,
                            const std::string& theObjectFeatureAttribute)
  {
    const std::set<AttributePtr>& aRefsList = theFeature->data()->refsToMe();
    std::set<AttributePtr>::const_iterator aIt;
    for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
      FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      if (aRefFeature->getKind() == theReferenceFeatureKind) {
        std::list<AttributePtr> anAttributes =
                         theFeature->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
        std::list<AttributePtr>::iterator anIter = anAttributes.begin();
        // it searches the first point of AttributeRefAtt
        std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;
        for(; anIter != anAttributes.end() && !aPointAttr.get(); anIter++) {
          AttributeRefAttrPtr aRefAttribute =
            std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
          if (aRefAttribute.get())
            aPointAttr = getPointOfRefAttr(aRefFeature.get(), aRefAttribute->id(),
                                           theObjectFeatureKind, theObjectFeatureAttribute);
        }
        if (aPointAttr.get()) {
          theAttributes.insert(aPointAttr);
        }
      }
    }
  }

  void getPointsInsideShape(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                            const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                            const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                            const std::shared_ptr<GeomAPI_Dir>& theDirX,
                            const std::shared_ptr<GeomAPI_Dir>& theDirY,
                            std::set<std::shared_ptr<GeomAPI_Pnt> >& thePoints)
  {
    std::set<std::shared_ptr<GeomDataAPI_Point2D> >::const_iterator anIt = theAttributes.begin(),
                                                            aLast = theAttributes.end();
    for (; anIt != aLast; anIt++) {
      std::shared_ptr<GeomDataAPI_Point2D> anAttribute = *anIt;
      std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = anAttribute->pnt();
      std::shared_ptr<GeomAPI_Pnt> aPnt = aPnt2d->to3D(theOrigin, theDirX, theDirY);
      std::shared_ptr<GeomAPI_Vertex> aVertexShape(new GeomAPI_Vertex(aPnt->x(), aPnt->y(), aPnt->z()));
      if (GeomAlgoAPI_ShapeTools::isSubShapeInsideShape(aVertexShape, theBaseShape))
        thePoints.insert(aPnt);
    }
  }


} // namespace ModelGeomAlgo_Point2D
