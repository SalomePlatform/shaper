// Copyright (C) 2017-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_AttributeBuilder.h
// Created: 10 Feb 2017
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_AttributeBuilder_H_
#define PlaneGCSSolver_AttributeBuilder_H_

#include <PlaneGCSSolver_EntityBuilder.h>

/** \class   PlaneGCSSolver_AttributeBuilder
 *  \ingroup Plugins
 *  \brief   Converts ModelAPI_Attribute to the entity applicable for PlaneGCS solver
 */
class PlaneGCSSolver_AttributeBuilder : public PlaneGCSSolver_EntityBuilder
{
public:
  PlaneGCSSolver_AttributeBuilder(PlaneGCSSolver_Storage* theStorage = 0);
  PlaneGCSSolver_AttributeBuilder(const StoragePtr& theStorage);

  /// \brief Converts an attribute to the solver's entity.
  ///        Double attributes and 2D points are supported only.
  /// \param theAttribute [in]  attribute to create
  /// \return Created wrapper of the attribute applicable for specific solver
  virtual EntityWrapperPtr createAttribute(AttributePtr theAttribute) override;

  /// \brief Blank. To be defined in derived class.
  virtual EntityWrapperPtr createFeature(FeaturePtr) override
  { return EntityWrapperPtr(); }

  /// \brief Blank. To be defined in derived class.
  virtual const std::list<GCSConstraintPtr>& constraints() const override;
};

#endif
