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

#ifndef SketchPlugin_MacroBSpline_H_
#define SketchPlugin_MacroBSpline_H_

#include <ModelAPI_IReentrant.h>

#include "SketchPlugin.h"

#include "SketchPlugin_SketchEntity.h"

#include <GeomAPI_IPresentable.h>

class GeomAPI_Circ2d;
class GeomAPI_Pnt2d;

/**\class SketchPlugin_MacroBSpline
 * \ingroup Plugins
 * \brief Feature for creation of the new B-spline in Sketch.
 */
class SketchPlugin_MacroBSpline : public SketchPlugin_SketchEntity,
                                  public GeomAPI_IPresentable,
                                  public ModelAPI_IReentrant
{
public:
  /// B-spline macro feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchMacroBSpline");
    return ID;
  }


  /// list of B-spline poles
  inline static const std::string& POLES_ID()
  {
    static const std::string ID("poles");
    return ID;
  }

  /// list of references of B-spline poles
  inline static const std::string& REF_POLES_ID()
  {
    static const std::string ID("poles_ref");
    return ID;
  }

  /// list of B-spline weights
  inline static const std::string& WEIGHTS_ID()
  {
    static const std::string ID("weights");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_MacroBSpline::ID();
    return MY_KIND;
  }

  /// \brief Request for initialization of data model of the feature: adding all attributes.
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;};

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;};

  /// Apply information of the message to current object. It fills reference object,
  /// tangent type and tangent point refence in case of tangent arc
  virtual std::string processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Use plugin manager for features creation
  SketchPlugin_MacroBSpline();

private:
  FeaturePtr createBSplineFeature();

  void createControlPolygon(FeaturePtr theBSpline, std::list<FeaturePtr>& thePoles);
  void constraintsForPoles(const std::list<FeaturePtr>& thePoles);

  int myDegree;
  bool myIsPeriodic;
};

#endif
