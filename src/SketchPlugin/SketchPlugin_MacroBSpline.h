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

class GeomDataAPI_Point2DArray;

/**\class SketchPlugin_MacroBSpline
 * \ingroup Plugins
 * \brief Feature for creation of the new B-spline in Sketch.
 */
class SketchPlugin_MacroBSpline : public SketchPlugin_SketchEntity,
                                  public GeomAPI_IPresentable
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

  /// flag attribute whether control polygon is need to be created
  inline static const std::string& CONTROL_POLYGON_ID()
  {
    static const std::string ID("need_control_poly");
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

  /// Use plugin manager for features creation
  SketchPlugin_MacroBSpline();

protected:
  SketchPlugin_MacroBSpline(bool isPeriodic);

private:
  FeaturePtr createBSplineFeature();

  void createControlPolygon(FeaturePtr theBSpline, std::list<FeaturePtr>& thePoles);
  void constraintsForPoles(const std::list<FeaturePtr>& thePoles);

  /// Create Point feature coincident with the B-spline pole
  static FeaturePtr createAuxiliaryPole(std::shared_ptr<GeomDataAPI_Point2DArray> theBSplinePoles,
                                        const int thePoleIndex);
  /// Create segment between consequtive B-spline poles
  static void createAuxiliarySegment(std::shared_ptr<GeomDataAPI_Point2DArray> theBSplinePoles,
                                     const int thePoleIndex1,
                                     const int thePoleIndex2);
  /// Set name of auxiliary feature representing the control polygon
  static void assignDefaultNameForAux(FeaturePtr theAuxFeature,
                                      std::shared_ptr<GeomDataAPI_Point2DArray> theBSplinePoles,
                                      const int thePoleIndex1,
                                      const int thePoleIndex2 = -1);
  friend class SketchPlugin_BSplineBase;

private:
  std::list<double> myKnots;
  std::list<int> myMultiplicities;
  int myDegree;
  bool myIsPeriodic;
};


/**\class SketchPlugin_MacroBSpline
* \ingroup Plugins
* \brief Feature for creation of the new B-spline in Sketch.
*/
class SketchPlugin_MacroBSplinePeriodic : public SketchPlugin_MacroBSpline
{
public:
  /// B-spline macro feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchMacroBSplinePeriodic");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    return SketchPlugin_MacroBSplinePeriodic::ID();
  }

  /// Use plugin manager for features creation
  SketchPlugin_MacroBSplinePeriodic() : SketchPlugin_MacroBSpline(true) {}
};

#endif
