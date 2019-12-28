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

#include <PlaneGCSSolver_AngleWrapper.h>
#include <PlaneGCSSolver_AttributeBuilder.h>
#include <PlaneGCSSolver_PointArrayWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_ScalarArrayWrapper.h>
#include <PlaneGCSSolver_BooleanWrapper.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_MultiRotation.h>

PlaneGCSSolver_AttributeBuilder::PlaneGCSSolver_AttributeBuilder(
    PlaneGCSSolver_Storage* theStorage)
  : PlaneGCSSolver_EntityBuilder(theStorage)
{
}

PlaneGCSSolver_AttributeBuilder::PlaneGCSSolver_AttributeBuilder(const StoragePtr& theStorage)
  : PlaneGCSSolver_EntityBuilder(
        std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(theStorage).get())
{
}

static double* createParameter(PlaneGCSSolver_Storage* theStorage)
{
  return theStorage ? theStorage->createParameter() : (new double(0));
}

static EntityWrapperPtr createBoolean(const AttributePtr& theAttribute)
{
  BooleanWrapperPtr aWrapper;
  AttributeBooleanPtr aBoolean =
      std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(theAttribute);
  if (aBoolean)
    aWrapper = BooleanWrapperPtr(new PlaneGCSSolver_BooleanWrapper(aBoolean->value()));
  return aWrapper;
}

static EntityWrapperPtr createScalar(const AttributePtr&     theAttribute,
                                     PlaneGCSSolver_Storage* theStorage)
{
  AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (!aScalar)
    return EntityWrapperPtr();

  ScalarWrapperPtr aWrapper;
  // following attributes should be converted from degrees to radians
  //  - value of the Angle constraint
  //  - angle of the Multi-Rotation constraint
  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());
  if ((theAttribute->id() == SketchPlugin_Constraint::VALUE() &&
      anOwner->getKind() == SketchPlugin_ConstraintAngle::ID()) ||
     (theAttribute->id() == SketchPlugin_MultiRotation::ANGLE_ID() &&
      anOwner->getKind() == SketchPlugin_MultiRotation::ID()))
    aWrapper = ScalarWrapperPtr(new PlaneGCSSolver_AngleWrapper(createParameter(theStorage)));
  else
    aWrapper = ScalarWrapperPtr(new PlaneGCSSolver_ScalarWrapper(createParameter(theStorage)));

  if (aScalar->isInitialized())
    aWrapper->setValue(aScalar->value());
  return aWrapper;
}

static EntityWrapperPtr createScalarArray(const AttributePtr&     theAttribute,
                                          PlaneGCSSolver_Storage* theStorage)
{
  AttributeDoubleArrayPtr anArray =
      std::dynamic_pointer_cast<ModelAPI_AttributeDoubleArray>(theAttribute);
  if (!anArray || !anArray->isInitialized())
    return EntityWrapperPtr();

  int aSize = anArray->size();
  GCS::VEC_pD aParameters;
  aParameters.reserve(aSize);
  for (int index = 0; index < aSize; ++index) {
    double* aParam = createParameter(theStorage);
    *aParam = anArray->value(index);
    aParameters.push_back(aParam);
  }

  return EntityWrapperPtr(new PlaneGCSSolver_ScalarArrayWrapper(aParameters));
}

static PointWrapperPtr createPoint(const GeomPnt2dPtr& thePoint, PlaneGCSSolver_Storage* theStorage)
{
  GCSPointPtr aNewPoint(new GCS::Point);

  aNewPoint->x = createParameter(theStorage);
  aNewPoint->y = createParameter(theStorage);
  if (thePoint) {
    *(aNewPoint->x) = thePoint->x();
    *(aNewPoint->y) = thePoint->y();
  }

  return PointWrapperPtr(new PlaneGCSSolver_PointWrapper(aNewPoint));
}

static EntityWrapperPtr createPoint(const AttributePtr&     theAttribute,
                                    PlaneGCSSolver_Storage* theStorage)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (!aPoint2D)
    return EntityWrapperPtr();

  GeomPnt2dPtr aPnt;
  if (aPoint2D->isInitialized())
    aPnt = aPoint2D->pnt();

  return createPoint(aPnt, theStorage);
}

static EntityWrapperPtr createPointArray(const AttributePtr& theAttribute,
                                         PlaneGCSSolver_Storage* theStorage)
{
  std::shared_ptr<GeomDataAPI_Point2DArray> aPointArray =
      std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(theAttribute);
  if (!aPointArray)
    return EntityWrapperPtr();

  int aSize = aPointArray->size();

  std::vector<PointWrapperPtr> aPointWrappers;
  aPointWrappers.reserve(aSize);
  for (int index = 0; index < aSize; ++index)
    aPointWrappers.push_back(createPoint(aPointArray->pnt(index), theStorage));

  return EntityWrapperPtr(new PlaneGCSSolver_PointArrayWrapper(aPointWrappers));
}

EntityWrapperPtr PlaneGCSSolver_AttributeBuilder::createAttribute(
    AttributePtr theAttribute)
{
  EntityWrapperPtr aResult;
  if (myStorage)
    aResult = myStorage->entity(theAttribute);
  if (!aResult)
    aResult = createPoint(theAttribute, myStorage);
  if (!aResult)
    aResult = createScalar(theAttribute, myStorage);
  if (!aResult)
    aResult = createBoolean(theAttribute);
  if (!aResult)
    aResult = createPointArray(theAttribute, myStorage);
  if (!aResult)
    aResult = createScalarArray(theAttribute, myStorage);
  if (aResult && !myStorage)
    aResult->setExternal(true);
  return aResult;
}
