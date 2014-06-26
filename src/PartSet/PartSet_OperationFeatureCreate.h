// File:        PartSet_OperationFeatureCreate.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationFeatureCreate_H
#define PartSet_OperationFeatureCreate_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <PartSet_Constants.h>

#include <QObject>

class GeomDataAPI_Point2D;
class QMouseEvent;
class QKeyEvent;

/*!
 \class PartSet_OperationFeatureCreate
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_OperationFeatureCreate : public PartSet_OperationSketchBase
{
  Q_OBJECT

public:
  /// Returns true if the feature with the given kind can be created by this operation
  /// \param theId the feature kind
  /// \return the boolean result
  static bool canProcessKind(const std::string& theId);

public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  /// \param theSketch the parent feature
  PartSet_OperationFeatureCreate(const QString& theId, QObject* theParent,
                                 FeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_OperationFeatureCreate();

  /// Verifies whether this operator can be commited.
  /// \return Returns TRUE if current operation can be committed, e.g. all parameters are filled
  virtual bool canBeCommitted() const;

  /// Returns that this operator can be started above already running one.
   /// The runned operation should be the sketch feature modified operation
  /// \param theOperation the previous running operation
  virtual bool isGranted(ModuleBase_IOperation* theOperation) const;

  /// Returns the operation local selection mode
  /// \param theFeature the feature object to get the selection mode
  /// \return the selection mode
  virtual std::list<int> getSelectionModes(FeaturePtr theFeature) const;

  /// Initializes the operation with previously created feature. It is used in sequental operations
  virtual void initFeature(FeaturePtr theFeature);

  /// Initialisation of operation with preliminary selection
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void initSelection(const std::list<XGUI_ViewerPrs>& theSelected,
    const std::list<XGUI_ViewerPrs>& theHighlighted);

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual FeaturePtr sketch() const;

  /// Gives the current selected objects to be processed by the operation
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
 virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                            const std::list<XGUI_ViewerPrs>& theSelected,
                            const std::list<XGUI_ViewerPrs>& theHighlighted);
  /// Gives the current mouse point in the viewer
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mouseMoved(QMouseEvent* theEvent, Handle_V3d_View theView);
  /// Processes the key pressed in the view
  /// \param theKey a key value
  virtual void keyReleased(const int theKey);

  virtual void keyReleased(std::string theName, QKeyEvent* theEvent);

public slots:
  /// Slots which listen the mode widget activation
  /// \param theWidget the model widget
  virtual void onWidgetActivated(ModuleBase_ModelWidget* theWidget);

protected:
  /// \brief Virtual method called when operation is started
  /// Virtual method called when operation started (see start() method for more description)
  /// After the parent operation body perform, set sketch feature to the created line feature
  virtual void startOperation();

  /// Virtual method called when operation aborted (see abort() method for more description)
  /// Before the feature is aborted, it should be hidden from the viewer
  virtual void abortOperation();

  /// Virtual method called when operation stopped - committed or aborted.
  /// Restore the multi selection state
  virtual void stopOperation();

  /// Virtual method called after operation committed (see commit() method for more description)
  virtual void afterCommitOperation();

  /// Creates an operation new feature
  /// In addition to the default realization it appends the created line feature to
  /// the sketch feature
  /// \param theFlushMessage the flag whether the create message should be flushed
  /// \returns the created feature
  virtual FeaturePtr createFeature(const bool theFlushMessage = true);

protected:
  /// Set value to the active widget
  /// \param theFeature the feature
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  /// \return true if the point is set
  bool setWidgetValue(FeaturePtr theFeature, double theX, double theY);

private:
  FeaturePtr myInitFeature; ///< the initial feature
  FeaturePtr mySketch; ///< the sketch of the feature

  ModuleBase_ModelWidget* myActiveWidget; ///< the active widget

  std::list<XGUI_ViewerPrs> myPreSelection;
};

#endif
