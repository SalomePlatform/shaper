// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef PartSet_WidgetPoint2D_H
#define PartSet_WidgetPoint2D_H

#include "PartSet.h"
#include "PartSet_MouseProcessor.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ViewerPrs.h>

#include <QObject>

#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>

class ModelAPI_Feature;
class ModelAPI_AttributeRefAttr;
class GeomDataAPI_Point2D;
class ModuleBase_IWorkshop;
class ModuleBase_ParamSpinBox;
class ModuleBase_IViewWindow;
class ModuleBase_LabelValue;
class PartSet_ExternalObjectsMgr;
class GeomAPI_Pnt2d;
class ModuleBase_IWorkshop;

class QGroupBox;
class QMouseEvent;

/**\class PartSet_WidgetPoint2D
 * \ingroup Modules
 * \brief Implementation of model widget to provide widget to input point 2d
 * In XML can be defined as folloung:
 * \code
 * <sketch-2dpoint_selector id="CircleCenter" title="Center" tooltip="Center coordinates"/>
 * \endcode
 */
class PARTSET_EXPORT PartSet_WidgetPoint2D : public ModuleBase_ModelWidget,
                                             public PartSet_MouseProcessor
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop a current workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetPoint2D(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                        const Config_WidgetAPI* theData);
  /// Destructor
  virtual ~PartSet_WidgetPoint2D();

  /// Fills given container with selection modes if the widget has it
  /// \param [out] theModuleSelectionModes module additional modes, -1 means all default modes
  /// \param theModes [out] a container of modes
  virtual void selectionModes(int& theModuleSelectionModes, QIntList& theModes);

  /// Checks if the selection presentation is valid in widget
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// Checks all attribute validators returns valid. It tries on the given selection
  /// to current attribute by setting the value inside and calling validators. After this,
  /// the previous attribute value is restored.The valid/invalid value is cashed.
  /// \param theValue a selected presentation in the view
  /// \param theAttribute the attribute
  /// \return a boolean value
  bool isValidSelectionForAttribute_(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue,
                                     const std::shared_ptr<ModelAPI_Attribute>& theAttribute);

  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  bool setSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped widget values
  /// \param theToValidate a validation flag
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

  /// Select the internal content if it can be selected. It is empty in the default realization
  //virtual void selectContent();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  //bool initFromPrevious(ObjectPtr theObject);

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// \returns the sketch instance
  CompositeFeaturePtr sketch() const { return mySketch; }

  /// Set sketch instance
  void setSketch(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Fill the widget values by given point
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  /// \returns True in case of success
  bool setPoint(double theX, double theY);

  /// Returns coordinate X currently defined in the control
  double x() const;

  /// Returns coordinate Y currently defined in the control
  double y() const;

  /// Returns true if the event is processed.
  virtual bool processEnter();

  /// Returns true if the attribute can be changed using the selected shapes in the viewer
  /// and creating a coincidence constraint to them. This control use them.
  virtual bool useSelectedShapes() const;

  /// Processing the mouse move event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  virtual void mouseMoved(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent);

  /// Processing the mouse release event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  virtual void mouseReleased(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent);

  /// Fill preselection used in mouseReleased
  //virtual void setPreSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePreSelected);
  virtual void setPreSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePreSelected,
                               ModuleBase_IViewWindow* theWnd,
                               QMouseEvent* theEvent);

  /// Return an object and geom shape by the viewer presentation
  /// \param thePrs a selection
  /// \param theObject an output object
  /// \param theShape a shape of the selection
  void getGeomSelection_(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue,
                         std::shared_ptr<ModelAPI_Object>& theObject,
                         std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Return reference to external objects manager
  /// \return manager instance
  PartSet_ExternalObjectsMgr* getExternalObjectMgr() const { return myExternalObjectMgr; }

signals:
  /// Signal about selection of an existing vertex from an object
  void vertexSelected();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

  /// Store current value in cashed value
  void storeCurentValue();

  /// Restore cashed value in the model attribute
  /// \return boolean state if the restored feature shoud be hidden
  bool restoreCurentValue();

  /// Fills the widget with default values
  /// \return true if the widget current value is reset
  virtual bool resetCustom();

  /// The methiod called when widget is activated
  virtual void activateCustom();

  //! Switch On/Off highlighting of the widget
  virtual void setHighlighted(bool isHighlighted);

  /// Returns true if the feature contains Point2D attribute with the same coordinates
  /// The attribute of the widget is not processed.
  /// \param theFeature a feature
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  /// \return boolean result
  bool isFeatureContainsPoint(const FeaturePtr& theFeature, double theX, double theY);

//private slots:
  /// Process value changed event
  //void onValuesChanged();

  /// Compute the feature default value and fill the controls with it
  /// or store the control value to the feature
  /// The widget is not initialize the attribute value in order to avoid the
  /// incorrect visualization in Sketch. E.g. by a line creation, the line should not
  /// be visualized immediatelly when the end point widget is activated.
  virtual void initializeValueByActivate();

 private:
   /// Creates constrains of the clicked point
   /// \param theClickedX the horizontal coordnate of the point
   /// \param theClickedY the vertical coordnate of the point
   bool setConstraintToPoint(double theClickedX, double theClickedY,
                             const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

   /// Create a coincidence constraint between the attribute and the parameter object
   /// \theObject a result object
   /// \return true if succed
   bool setConstraintToObject(const ObjectPtr& theObject);

   /// Process selected value
   /// \param theValue the selected object
   /// \param theX X coordinate of clicked point
   /// \param theY Y coordinate of clicked point
   void processSelection(const ModuleBase_ViewerPrsPtr& theValue, double theX, double theY);


public:
   /// Returns if the feature is an orphan point, circle or an arc. Returns true if it
   /// has no a coincident to other lines. It processes point, circle and arc features
   /// In circle an arc features, only centers are processed, for other points, it returns
   /// that the point is not an orphan.
   /// \param theFeature a checked feature
   /// \param theSketch a sketch
   /// \param theX an X coordinate of the point
   /// \param theY an Y coordinate of the point
   /// \return boolean result
   static bool isOrphanPoint(const FeaturePtr& theFeature, const CompositeFeaturePtr& theSketch,
                             double theX, double theY);

   /// Explode the given shape by vertices and found closed to the point vertes
   /// \param theShape a shape to be exploded
   /// \param thePoint a point
   /// \return boolean value
   static bool shapeExploreHasVertex(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                     const std::shared_ptr<GeomAPI_Pnt2d>& thePoint,
                                     const CompositeFeaturePtr& theSketch);

  /// Finds in the feature's sub-features first Point2D attribute with the given point coordinates
  /// \param theFeature a feature with sub-feature attributes
  /// \param thePoint a point to provided searched coordinates
  /// \return found point or null
  static std::shared_ptr<GeomDataAPI_Point2D> findFirstEqualPointInArgumentFeatures(
                  const FeaturePtr& theFeature, const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Finds in the feature first Point2D attribute with the given point coordinates
  /// \param theFeature a feature with point attributes
  /// \param thePoint a point to provided searched coordinates
  /// \return found point or null
  static std::shared_ptr<GeomDataAPI_Point2D> findFirstEqualPoint(const FeaturePtr& theFeature,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Finds in the sketch attribute point of any feature that is nearest to the given point
  /// \param theSketch a sketch, that is a container of features
  /// \param theSkipFeature a feature that do not participate in the search
  /// \param thePoint a point to provided searched coordinates
  /// \return found point or null
  static std::shared_ptr<GeomDataAPI_Point2D> findFirstEqualPointInSketch(
                                       const CompositeFeaturePtr& theSketch,
                                       const FeaturePtr& theSkipFeature,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

private:
  /// Returns attribute reference if the key is defined in XML definition of this control
  /// \return found attribute or null
  std::shared_ptr<ModelAPI_AttributeRefAttr> attributeRefAttr() const;

  /// Finds first equal point attribute in sketch and set it to reference attribute
  /// \param theClickedX the horizontal coordnate of the point
  /// \param theClickedY the vertical coordnate of the point
  void fillRefAttribute(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);
  void fillRefAttribute(double theClickedX, double theClickedY,
                        const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);
  void fillRefAttribute(const ObjectPtr& theObject);

  ObjectPtr getGeomSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

protected:
  ModuleBase_IWorkshop* myWorkshop; ///< workshop

private:
  QGroupBox* myGroupBox;  ///< the parent group box for all intenal widgets
  //ModuleBase_ParamSpinBox* myXSpin;  ///< the spin box for the X coordinate
  //ModuleBase_ParamSpinBox* myYSpin;  ///< the spin box for the Y coordinate
  ModuleBase_LabelValue* myXSpin; ///< the label for the X coordinate
  ModuleBase_LabelValue* myYSpin; ///< the label for the Y coordinate
  PartSet_ExternalObjectsMgr* myExternalObjectMgr; ///< reference to external objects manager

  /// value used as selection in mouse release method
  std::shared_ptr<ModuleBase_ViewerPrs> myPreSelected;

  /// it is important during restart operation
  CompositeFeaturePtr mySketch;

  std::string myRefAttribute; /// if not empty, coincidences are not set but attribute is filled
  bool myValueIsCashed; /// boolean state if the value is cashed during value state change
  bool myIsFeatureVisibleInCash; /// boolean value if the feature was visible when cash if filled
  double myXValueInCash; /// the cashed X value during value state change
  double myYValueInCash; /// the cashed Y value during value state change
};

#endif
