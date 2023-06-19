// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef SketchSolver_ConstraintMultiTranslation_H_
#define SketchSolver_ConstraintMultiTranslation_H_

#include <SketchSolver_ConstraintMulti.h>

#include "GeomDataAPI_Point2D.h"

/** \class   SketchSolver_ConstraintMultiTranslation
 *  \ingroup Plugins
 *  \brief   Convert translated features to the list of SolveSpace constraints
 */
class SketchSolver_ConstraintMultiTranslation : public SketchSolver_ConstraintMulti
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMultiTranslation(ConstraintPtr theConstraint) :
      SketchSolver_ConstraintMulti(theConstraint)
  {}

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of translated entities
  /// \param[out] theStartPoint start point of translation
  /// \param[out] theEndPoint   final point of translation
  /// \param[out] theFullValue  applying translation using the distance as a full or single value
  /// \param[out] theEntities   list of base entities
  void getAttributes(EntityWrapperPtr& theStartPoint, EntityWrapperPtr& theEndPoint,
                     bool& theFullValue, std::list<EntityWrapperPtr>& theEntities);

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

  /// \brief Update parameters (called from base class)
  virtual void updateLocal();

private:
  /// \brief Convert absolute coordinates to relative coordinates
  virtual void getRelative(double theAbsX, double theAbsY,
                           double& theRelX, double& theRelY);
  /// \brief Convert relative coordinates to absolute coordinates
  virtual void getAbsolute(double theRelX, double theRelY,
                           double& theAbsX, double& theAbsY);
  /// \brief Apply transformation for relative coordinates
  virtual void transformRelative(double& theX, double& theY);

  /// \brief Returns name of NUMBER_OF_COPIES parameter for corresponding feature
  virtual const std::string& nameNbObjects();

private:
  AttributePoint2DPtr myStartPointAttribute;
  AttributePoint2DPtr myEndPointAttribute;

  double myDelta[2]; ///< increment of translation
};

#endif
