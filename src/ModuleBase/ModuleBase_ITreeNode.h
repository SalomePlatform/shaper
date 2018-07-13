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

#ifndef ModuleBase_ITreeNode_H
#define ModuleBase_ITreeNode_H

#include "ModuleBase.h"

#include <ModelAPI_Object.h>

#include <QList>
#include <QString>
#include <QIcon>

class ModuleBase_ITreeNode;

typedef QList<ModuleBase_ITreeNode*> QTreeNodesList;

class ModuleBase_ITreeNode
{
public:
  /// Default constructor
  ModuleBase_ITreeNode() : myParent(0) {}

  /// Returns name of the node
  virtual QString name() const { return "Item"; }

  /// Returns icon of the node
  virtual QIcon icon() const { return QIcon(); }

  /// Returns foreground color of the node
  virtual QColor color() const { return Qt::black;  }

  /// Returns properties flag of the item
  virtual Qt::ItemFlags falg() const { return Qt::ItemIsSelectable | Qt::ItemIsEnabled; }

  /// Returns parent node of the current node
  ModuleBase_ITreeNode* parent() const { return myParent; }

  /// Returns list of the node children
  QTreeNodesList children() const { return myChildren; }

  /// Returns object referenced by the node (can be null)
  virtual ObjectPtr object() const { return ObjectPtr(); }

protected:
  ModuleBase_ITreeNode* myParent; //!< Parent of the node
  QTreeNodesList myChildren; //!< Children of the node
};

#endif