// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_BSplineBase_H_
#define SketchPlugin_BSplineBase_H_

#include <SketchPlugin.h>
#include <SketchPlugin_SketchEntity.h>

/**\class SketchPlugin_BSplineBase
 * \ingroup Plugins
 * \brief Base class for B-spline curves in the sketch.
 */
class SketchPlugin_BSplineBase : public SketchPlugin_SketchEntity
{
public:
  /// list of B-spline poles
  inline static const std::string& POLES_ID()
  {
    static const std::string ID("poles");
    return ID;
  }

  /// list of B-spline weights
  inline static const std::string& WEIGHTS_ID()
  {
    static const std::string ID("weights");
    return ID;
  }

  /// attribute to store the degree of B-spline
  inline static const std::string& DEGREE_ID()
  {
    static const std::string ID("degree");
    return ID;
  }

  /// list of B-spline knots
  inline static const std::string& KNOTS_ID()
  {
    static const std::string ID("knots");
    return ID;
  }

  /// list of B-spline multiplicities
  inline static const std::string& MULTS_ID()
  {
    static const std::string ID("multiplicities");
    return ID;
  }

  /// name for add pole action
  inline static const std::string& ADD_POLE_ACTION_ID()
  {
    static const std::string ID("AddPole");
    return ID;
  }

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Updates the B-spline curve.
  /// \param[in] theActionId action key id (in following form: Action#Index)
  /// \return \c false in case the action not performed.
  SKETCHPLUGIN_EXPORT virtual bool customAction(const std::string& theActionId);

protected:
  /// Called from the derived class
  SketchPlugin_BSplineBase();

  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

  /// \brief Return \c true if the B-spline curve is periodic
  virtual bool isPeriodic() const = 0;

  /// Add new pole after the pole with the given index
  bool addPole(const int theAfter);
};

#endif
