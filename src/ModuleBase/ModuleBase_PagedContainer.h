// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef MODULEBASE_PAGEDCONTAINER_H_
#define MODULEBASE_PAGEDCONTAINER_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

class ModuleBase_PageBase;

/**
* \ingroup GUI
* This is an abstract interface to be used for not model container widget such as switch or tool box.
*/
class MODULEBASE_EXPORT ModuleBase_PagedContainer : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
   /// A constructor
   /// \param theParent a parent widget
   /// \param theData a data of the widget
  ModuleBase_PagedContainer(QWidget* theParent, const Config_WidgetAPI* theData);
  virtual ~ModuleBase_PagedContainer();

  /// Add a new page
  /// \param theWidget a page object
  /// \param theName a name of the page
  /// \param theCaseId an Id of the page
  /// \param theIcon aqn Icon of the page
  virtual int addPage( ModuleBase_PageBase* theWidget,
                       const QString& theName,
                       const QString& theCaseId,
                       const QPixmap& theIcon );

  /// Redefinition of virtual function
  virtual QList<QWidget*> getControls() const;

  /// Redefinition of virtual function
  virtual bool focusTo();

  /// Redefinition of virtual function
  virtual void setHighlighted(bool isHighlighted);

  /// Redefinition of virtual function
  virtual void enableFocusProcessing();

 protected:
   /// Returns index of current page
  virtual int currentPageIndex() const = 0;

  /// Set current page by index
  virtual void setCurrentPageIndex(int ) = 0;

  /// Redefinition of virtual function
  virtual void activateCustom();

  /// Redefinition of virtual function
  virtual bool storeValueCustom();

  /// Redefinition of virtual function
  virtual bool restoreValueCustom();

 protected slots:
   /// A slot called on page change
  void onPageChanged();

 private:
  bool myIsFocusOnCurrentPage;
  QStringList myCaseIds;
  QList<ModuleBase_PageBase*> myPages;

};

#endif /* MODULEBASE_PAGEDCONTAINER_H_ */
