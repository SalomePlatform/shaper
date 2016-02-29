#include <SketchSolver_ConstraintCollinear.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_Line.h>

SketchSolver_ConstraintCollinear::SketchSolver_ConstraintCollinear(ConstraintPtr theConstraint)
  : SketchSolver_Constraint(theConstraint)
{
}

void SketchSolver_ConstraintCollinear::notifyCoincidenceChanged(
    EntityWrapperPtr theCoincAttr1,
    EntityWrapperPtr theCoincAttr2)
{
  bool used = true;

  // obtain IDs of all boundary points of lines
  EntityID aPointIDs[4];
  for (int i = 0; i < 2; ++i) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        myBaseConstraint->attribute(SketchPlugin_Constraint::ATTRIBUTE(i)));
    if (!aRefAttr->object())
      continue;
    FeaturePtr aLine = ModelAPI_Feature::feature(aRefAttr->object());
    AttributePtr aLinePt = aLine->attribute(SketchPlugin_Line::START_ID());
    aPointIDs[2*i] = myStorage->entity(aLinePt)->id();
    aLinePt = aLine->attribute(SketchPlugin_Line::END_ID());
    aPointIDs[2*i + 1] = myStorage->entity(aLinePt)->id();
  }

  EntityWrapperPtr anAttrs[2] = {theCoincAttr1, theCoincAttr2};
  for (int i = 0; i < 2 && used; ++i) {
    if (anAttrs[i]->baseAttribute())
      used = used && isUsed(anAttrs[i]->baseAttribute());
    else
      used = used && isUsed(anAttrs[i]->baseFeature());

    if (!used) {
      if (anAttrs[i]->type() == ENTITY_POINT) {
        EntityID anID = anAttrs[i]->id();
        for (int j = 0; j < 4; ++j)
          if (anID == aPointIDs[j]) {
            used = true;
            break;
          }
      }
    }
  }

  if (used) {
    remove();
    process();
  }
}
