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

#include "SHAPERGUI_CheckBackup.h"
#include <fstream>
#include <QDir>
#include <QFileInfo>
#include <QStringList>



SHAPERGUI_CheckBackup::SHAPERGUI_CheckBackup(const QString &theFolder, const QString &theBaseName)
  : myProcess(nullptr)
  , myFolder(theFolder)
  , myBaseName(theBaseName)
{
}


int SHAPERGUI_CheckBackup::run(const QString &theTestScript)
{
  int aResult = 0;
  if (myProcess)
    return 64;

  QString aProgName = std::getenv("PYTHONBIN");
  if (aProgName.isEmpty())
    aProgName = "python3";

  const QChar aSep = QDir::separator();

  QString testBackup("test_backup.py");
  QStringList dirs;
  dirs << QString(std::getenv("SHAPER_ROOT_DIR"))
       << QString("bin")
       << QString("salome")
       << testBackup;
  QString scriptName = dirs.join( QDir::separator() );

  QStringList args;
  args << scriptName << myFolder << myBaseName << theTestScript;

  myProcess = new QProcess(this);
  if (!myProcess)
    return 65;

  // connect(myProcess, SIGNAL(started()), this, SLOT(procStarted()));
  // connect(myProcess, SIGNAL(error(QProcess::ProcessError)), this,
  //         SLOT(procError(QProcess::ProcessError)));
  // connect(myProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
  //         SLOT(procFinished(int, QProcess::ExitStatus)));

  // Start a python script (test_backup.py), which itself
  //   * launches a SALOME process
  //   * opens the previously backed up HDF study
  myProcess->start(aProgName, args);
  myProcess->waitForFinished(300000);
  QProcess::ExitStatus exitStat = myProcess->exitStatus(); // 0=NormalExit, 1=CrashExit
  int exitCode = myProcess->exitCode();
  if (exitStat == QProcess::NormalExit && exitCode != 0) 
    aResult = exitCode;
  else if (exitStat == QProcess::CrashExit)
    aResult = 66;
  myProcess->deleteLater();

  // Check the output of the log file
  if (aResult == 0)
  {
    std::ifstream log((QStringList() << myFolder << myBaseName+"_valid.log").join(aSep).toStdString().c_str());
    if (log)
    {
      uint8_t testFlag = 0;
      std::string line;
      while (std::getline(log, line))
      {
        if (line.find("HDF Test - ") == 0)
        {
          testFlag |= 0x1; // 0x01 = HDF Test performed
          std::string strResult = line.substr(11);
          if (strResult.find("PASSED") == 0)
          {
            if (testFlag == 0x03)
              break;
          }
          else if (strResult.find("FAILED") == 0)
          {
            aResult = 67;
            break;
          }
          else
          {
            aResult = 68;
            break;
          }
        }
        else if (line.find("PY Test - ") == 0)
        {
          testFlag |= 0x2; // 0x02 = PY Test performed
          std::string strResult = line.substr(10);
          if (strResult.find("PASSED") == 0)
          {
            if (testFlag == 0x03)
              break;
          }
          else if (strResult.find("FAILED") == 0)
          {
            aResult = 69;
            break;
          }
          else
          {
            aResult = 70;
            break;
          }
        }
      }
      if (aResult == 0 && testFlag != 0x03)
      {
        // Not all tests were performed or they were interrupted
        switch (testFlag)
        {
          case 0x00:  aResult = 71; break;
          case 0x01:  aResult = 72; break;
          case 0x02:  aResult = 73; break;
        }
      }
    }
    else
    {
      aResult = 74; // log file not found
    }
  }

  return aResult;
}


void SHAPERGUI_CheckBackup::procStarted()
{
}


void SHAPERGUI_CheckBackup::procFinished(int code, QProcess::ExitStatus stat)
{
}


void SHAPERGUI_CheckBackup::procError(QProcess::ProcessError err)
{
}
