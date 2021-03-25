// Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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

#ifndef PartSet_WidgetBSplinePoints_H
#define PartSet_WidgetBSplinePoints_H

#include "PartSet.h"
#include "PartSet_MouseProcessor.h"

#include <ModuleBase_ModelWidget.h>

#include <QObject>

class GeomAPI_Pnt2d;
class ModelAPI_CompositeFeature;
class ModuleBase_LabelValue;
class PartSet_ExternalObjectsMgr;
class QGroupBox;
class QScrollArea;

/**\class PartSet_WidgetBSplinePoints
 * \ingroup Modules
 * \brief Implementation of model widget to provide widget to input a list of 2D poles
 *        of B-spline curve in association with weights
 * In XML can be defined as following:
 * \code
 * <sketch-bspline_selector id="poles" weights="weights"/>
 * \endcode
 */
class PARTSET_EXPORT PartSet_WidgetBSplinePoints : public ModuleBase_ModelWidget,
                                                   public PartSet_MouseProcessor
{
Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop a current workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetBSplinePoints(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                              const Config_WidgetAPI* theData);
  /// Destructor
  virtual ~PartSet_WidgetBSplinePoints();

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

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// \returns the sketch instance
  std::shared_ptr<ModelAPI_CompositeFeature> sketch() const { return mySketch; }

  /// Set sketch instance
  void setSketch(std::shared_ptr<ModelAPI_CompositeFeature> theSketch) { mySketch = theSketch; }

  /// Fill the widget values by given point
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  /// \returns True in case of success
  bool setPoint(double theX, double theY);

  /// Returns true if the event is processed.
  virtual bool processEscape();

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

private:
  /// Create labels for the next B-spline point
  void createNextPoint();
  /// Remove labels for the last B-spline point
  void removeLastPoint();

  /// Save B-spline poles and weights to corresponding attributes
  void storePolesAndWeights() const;

  /// Returns attribute reference if the key is defined in XML definition of this control
  /// \return found attribute or null
  std::shared_ptr<ModelAPI_AttributeRefAttrList> attributeRefAttrList() const;

  void fillRefAttribute(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);
  void fillRefAttribute(std::shared_ptr<GeomAPI_Pnt2d> theClickedPoint,
                        const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);
  void fillRefAttribute(const ObjectPtr& theObject);

  ObjectPtr getGeomSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

protected:
  ModuleBase_IWorkshop* myWorkshop; ///< workshop

private:
  QGroupBox* myBox;
  QWidget* myGroupBox;  ///< the parent group box for all intenal widgets
  QScrollArea* myScrollArea;
  std::vector<ModuleBase_LabelValue*> myXSpin; ///< the label for the X coordinate
  std::vector<ModuleBase_LabelValue*> myYSpin; ///< the label for the Y coordinate
  PartSet_ExternalObjectsMgr* myExternalObjectMgr; ///< reference to external objects manager

  /// value used as selection in mouse release method
  std::shared_ptr<ModuleBase_ViewerPrs> myPreSelected;

  /// it is important during restart operation
  CompositeFeaturePtr mySketch;

  std::string myRefAttribute; /// if not empty, coincidences are not set but attribute is filled

  bool myValueIsCashed; /// boolean state if the value is cashed during value state change
  bool myIsFeatureVisibleInCash; /// boolean value if the feature was visible when cash if filled
  std::vector<double> myXValueInCash; /// the cashed X value during value state change
  std::vector<double> myYValueInCash; /// the cashed Y value during value state change
  std::vector<double> myWeightInCash; /// the cached Weight value during valude state change

  std::string myWeightsAttr;

  int myPointIndex; /// index of the changing point

  bool myFinished; /// \c true if building the B-spline is finished (escape pressed)
};

#endif
