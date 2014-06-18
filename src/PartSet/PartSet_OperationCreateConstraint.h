// File:        PartSet_OperationCreateConstraint.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationCreateConstraint_H
#define PartSet_OperationCreateConstraint_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <PartSet_Constants.h>

#include <QObject>

class PartSet_FeaturePrs;
class GeomDataAPI_Point2D;
class QMouseEvent;
class QKeyEvent;

/*!
 \class PartSet_OperationCreateConstraint
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_OperationCreateConstraint : public PartSet_OperationSketchBase
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
  PartSet_OperationCreateConstraint(const QString& theId, QObject* theParent,
                                 FeaturePtr theSketch);
  /// Destructor
  virtual ~PartSet_OperationCreateConstraint();

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

  /// Initializes some fields accorging to the feature
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void init(FeaturePtr theFeature,
                    const std::list<XGUI_ViewerPrs>& theSelected,
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
  ///< Set the point selection mode. Emit signal about focus change if necessary.
  /// \param theMode a new selection mode
  /// \param isToEmitSignal the neccessity to emit signal
  void setPointSelectionMode(const PartSet_SelectionMode& theMode,
                             const bool isToEmitSignal = true);

  /// Show the value editor
  /// \param theEvent to get the mouse cursor position
  /// \param theValue an editor value
  void showEditor(QMouseEvent* theEvent, double theValue);

protected slots:
  /// SLOT, that listens the value edited signal and set the new value to the feature
  /// \param theValue the editor value
  void onEditStopped(double theValue);

private:
  boost::shared_ptr<PartSet_FeaturePrs> myFeaturePrs; ///< the feature presentation
  FeaturePtr myInitFeature; ///< the initial feature
  PartSet_SelectionMode myPointSelectionMode; ///< point selection mode
};

#endif
