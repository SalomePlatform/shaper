// Copyright (C) 2024-2026  CEA, EDF, OPEN CASCADE SAS
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

#ifndef SHAPERGUI_CHECKBACKUP_H
#define SHAPERGUI_CHECKBACKUP_H

#include "SHAPER_SHAPERGUI.h"
#include <QProcess>


class SHAPERGUI_EXPORT SHAPERGUI_CheckBackup : public QObject
{
  Q_OBJECT
public:
  SHAPERGUI_CheckBackup(const QString &theFolder, const QString &theBaseName);
  int run(const QString &theTestScript);

private slots:
  void procStarted();
  void procFinished(int, QProcess::ExitStatus);
  void procError(QProcess::ProcessError);

private:
  QProcess* myProcess;
  QString   myFolder;
  QString   myBaseName;
};

#endif /* SHAPERGUI_CHECKBACKUP_H */
