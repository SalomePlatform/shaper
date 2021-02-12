// Copyright (C) 2021  CEA/DEN, EDF R&D
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

#include <QProcessEnvironment>
#include <QDir>
#include <QProcess>

#include <iostream>

int main(int argc, char *argv[])
{
  constexpr char GUI_ROOT_DIR[]="GUI_ROOT_DIR";
  constexpr char SHAPER_ROOT_DIR[]="SHAPER_ROOT_DIR";
  constexpr char LIGHTAPPCONFIG[]="LightAppConfig";
  QProcessEnvironment pe(QProcessEnvironment::systemEnvironment());
  if(!pe.contains(SHAPER_ROOT_DIR))
    {
      std::cerr << SHAPER_ROOT_DIR << " is not defined in your environment !" << std::endl;
      return 1;
    }
  if(!pe.contains(GUI_ROOT_DIR))
    {
      std::cerr << GUI_ROOT_DIR << " is not defined in your environment !" << std::endl;
      return 1;
    }
  // fill LightAppConfig env var
  QString gui_root_dir( QDir::fromNativeSeparators(pe.value(GUI_ROOT_DIR)) );
  QString shaper_root_dir( QDir::fromNativeSeparators(pe.value(SHAPER_ROOT_DIR)) );
  QString lightappconfig_val( QString("%1:%2")
  .arg( QDir::toNativeSeparators( QString("%1/share/salome/resources/gui").arg(gui_root_dir) ) )
  .arg( QDir::toNativeSeparators( QString("%1/share/salome/resources/shaper").arg(shaper_root_dir) ) ) );
  pe.insert(LIGHTAPPCONFIG,lightappconfig_val);
  //tells shutup to salome.salome_init invoked at shaper engine ignition
  pe.insert("SALOME_EMB_SERVANT","1");
  //
  QProcess proc;
  proc.setProcessEnvironment(pe);
  proc.setProgram("suitexe");
  proc.setArguments({"--modules=SHAPER"});
  proc.setProcessChannelMode( QProcess::ForwardedErrorChannel );
  proc.start();
  proc.waitForFinished();
  return proc.exitCode();
}
