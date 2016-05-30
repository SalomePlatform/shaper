#include <SketchSolver_ConstraintMiddle.h>

#include <SketchSolver_Builder.h>
#include <SketchSolver_Manager.h>

#include <GeomAPI_XY.h>

SketchSolver_ConstraintMiddle::SketchSolver_ConstraintMiddle(ConstraintPtr theConstraint)
  : SketchSolver_Constraint(theConstraint)
{
}

void SketchSolver_ConstraintMiddle::notifyCoincidenceChanged(
    EntityWrapperPtr theCoincAttr1,
    EntityWrapperPtr theCoincAttr2)
{
  // Check the coincidence between point and line has been changed
  AttributePtr aPoint;
  FeaturePtr aLine;
  EntityWrapperPtr anEntities[2] = {theCoincAttr1, theCoincAttr2};
  for (int i = 0; i < 2; ++i) {
    if (anEntities[i]->type() == ENTITY_POINT)
      aPoint = anEntities[i]->baseAttribute();
    else if (anEntities[i]->type() == ENTITY_LINE)
      aLine = anEntities[i]->baseFeature();
  }
  if (!aPoint || !aLine)
    return;

  // Check the attributes of middle-point constraint are the same point and line
  bool isSameAttr = true;
  for (int i = 0; i < 2 && isSameAttr; ++i) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        myBaseConstraint->attribute(SketchPlugin_Constraint::ATTRIBUTE(i)));
    if (aRefAttr->isObject()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
      isSameAttr = (aFeature == aLine);
    } else
      isSameAttr = (aRefAttr->attr() == aPoint);
  }

  if (isSameAttr) {
    remove();
    process();
  }
}

void SketchSolver_ConstraintMiddle::adjustConstraint()
{
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint).front();
  const std::list<EntityWrapperPtr>& aSubs = aConstraint->entities();
  std::shared_ptr<GeomAPI_Pnt2d> aMidPoint, aStart, aEnd;
  std::list<EntityWrapperPtr>::const_iterator aSIt = aSubs.begin();
  for (; aSIt != aSubs.end(); ++aSIt) {
    if ((*aSIt)->type() == ENTITY_POINT)
      aMidPoint = aBuilder->point(*aSIt);
    else if ((*aSIt)->type() == ENTITY_LINE) {
      const std::list<EntityWrapperPtr>& aLinePoints = (*aSIt)->subEntities();
      aStart = aBuilder->point(aLinePoints.front());
      aEnd = aBuilder->point(aLinePoints.back());
    }
  }

  if (aMidPoint && aStart && aEnd) {
    std::shared_ptr<GeomAPI_XY> aMP = aMidPoint->xy();
    double aDot = aMP->decreased(aStart->xy())->dot(aMP->decreased(aEnd->xy()));
    if (aDot > 0.0) {
      aBuilder->adjustConstraint(aConstraint);
      myStorage->addConstraint(myBaseConstraint, aConstraint);
    }
  }
}
