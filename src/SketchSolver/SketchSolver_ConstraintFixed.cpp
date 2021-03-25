// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <SketchSolver_ConstraintFixed.h>
#include <SketchSolver_Error.h>

#include <PlaneGCSSolver_ConstraintWrapper.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_EntityDestroyer.h>
#include <PlaneGCSSolver_FeatureBuilder.h>
#include <PlaneGCSSolver_PointWrapper.h>

#include <GeomDataAPI_Point2D.h>
#include <SketchPlugin_Feature.h>

/// \brief Get list of parameters of current entity
static GCS::VEC_pD toParameters(const EntityWrapperPtr& theEntity);


SketchSolver_ConstraintFixed::SketchSolver_ConstraintFixed(ConstraintPtr theConstraint)
  : SketchSolver_Constraint(theConstraint)
{
  myType = CONSTRAINT_FIXED;
}

void SketchSolver_ConstraintFixed::blockEvents(bool isBlocked)
{
  SketchSolver_Constraint::blockEvents(isBlocked);
}

void SketchSolver_ConstraintFixed::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aBaseEntity = entityToFix();
  if (!aBaseEntity)
    myErrorMsg = SketchSolver_Error::ALREADY_FIXED();
  if (!myErrorMsg.empty())
    return;

  ConstraintWrapperPtr aConstraint = fixFeature(aBaseEntity);
  myStorage->addConstraint(myBaseConstraint, aConstraint);
}

ConstraintWrapperPtr SketchSolver_ConstraintFixed::fixFeature(EntityWrapperPtr theFeature)
{
  GCS::VEC_pD aParameters = toParameters(theFeature);

  // Fix given list of parameters
  std::list<GCSConstraintPtr> aConstraints;
  myFixedValues.reserve(aParameters.size());
  GCS::VEC_pD::const_iterator anIt = aParameters.begin();
  for (int i = 0; anIt != aParameters.end(); ++anIt, ++i) {
    myFixedValues.push_back(**anIt);
    aConstraints.push_back(
        GCSConstraintPtr(new GCS::ConstraintEqual(&myFixedValues[i], *anIt)));
  }

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aConstraints, getType()));
}

EntityWrapperPtr SketchSolver_ConstraintFixed::entityToFix()
{
  // Constraint Fixed is added by user.
  // Get the attribute of constraint (it should be alone in the list of constraints).
  EntityWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anAttributes;
  SketchSolver_Constraint::getAttributes(aValue, anAttributes);
  std::vector<EntityWrapperPtr>::const_iterator anIt = anAttributes.begin();
  for (; anIt != anAttributes.end(); ++anIt)
    if (*anIt)
      return *anIt;
  return EntityWrapperPtr();
}




// ==================     Auxiliary functions     ==================
GCS::VEC_pD toParameters(const EntityWrapperPtr& theEntity)
{
  GCS::VEC_pD aParameters;
  if (!theEntity)
    return aParameters;

  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> anEntity =
      std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(theEntity);

  // Collect parameters for each type of entity
  switch (theEntity->type()) {
  case ENTITY_POINT: {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theEntity);
    aParameters.push_back(aPoint->point()->x);
    aParameters.push_back(aPoint->point()->y);
    break;
    }
  case ENTITY_LINE: {
    std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(anEntity->entity());
    aParameters.push_back(aLine->p1.x);
    aParameters.push_back(aLine->p1.y);
    aParameters.push_back(aLine->p2.x);
    aParameters.push_back(aLine->p2.y);
    break;
    }
  case ENTITY_CIRCLE: {
    std::shared_ptr<GCS::Circle> aCircle =
        std::dynamic_pointer_cast<GCS::Circle>(anEntity->entity());
    aParameters.push_back(aCircle->center.x);
    aParameters.push_back(aCircle->center.y);
    aParameters.push_back(aCircle->rad);
    break;
    }
  case ENTITY_ARC: {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anEntity->entity());
    aParameters.push_back(anArc->center.x);
    aParameters.push_back(anArc->center.y);
    aParameters.push_back(anArc->rad);
    aParameters.push_back(anArc->startAngle);
    aParameters.push_back(anArc->endAngle);
    break;
    }
  case ENTITY_ELLIPSE: {
    std::shared_ptr<GCS::Ellipse> anEllipse =
        std::dynamic_pointer_cast<GCS::Ellipse>(anEntity->entity());
    aParameters.push_back(anEllipse->center.x);
    aParameters.push_back(anEllipse->center.y);
    aParameters.push_back(anEllipse->focus1.x);
    aParameters.push_back(anEllipse->focus1.y);
    aParameters.push_back(anEllipse->radmin);
    break;
    }
  case ENTITY_ELLIPTIC_ARC: {
    std::shared_ptr<GCS::ArcOfEllipse> anEllArc =
        std::dynamic_pointer_cast<GCS::ArcOfEllipse>(anEntity->entity());
    aParameters.push_back(anEllArc->center.x);
    aParameters.push_back(anEllArc->center.y);
    aParameters.push_back(anEllArc->focus1.x);
    aParameters.push_back(anEllArc->focus1.y);
    aParameters.push_back(anEllArc->radmin);
    aParameters.push_back(anEllArc->startAngle);
    aParameters.push_back(anEllArc->endAngle);
    break;
    }
  case ENTITY_BSPLINE: {
    std::shared_ptr<GCS::BSpline> aBSpline =
        std::dynamic_pointer_cast<GCS::BSpline>(anEntity->entity());
    for (GCS::VEC_P::iterator anIt = aBSpline->poles.begin();
         anIt != aBSpline->poles.end(); ++anIt) {
      aParameters.push_back(anIt->x);
      aParameters.push_back(anIt->y);
    }
    break;
  }
  default:
    break;
  }

  return aParameters;
}
