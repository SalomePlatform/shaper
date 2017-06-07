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

#include <XGUI_ColorDialog.h>

#include <ModelAPI_Tools.h>

#include <QtxColorButton.h>

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>
#include <QDialogButtonBox>

XGUI_ColorDialog::XGUI_ColorDialog(QWidget* theParent)
  : QDialog(theParent)
{
  setWindowTitle(tr("Color"));
  QGridLayout* aLay = new QGridLayout(this);

  QRadioButton* aRandomChoiceBtn = new QRadioButton(this);
  QRadioButton* aColorChoiceBtn = new QRadioButton(this);
  aColorChoiceBtn->setChecked(true);
  myButtonGroup = new QButtonGroup(this);
  myButtonGroup->setExclusive(true);
  myButtonGroup->addButton(aColorChoiceBtn, 0);
  myButtonGroup->addButton(aRandomChoiceBtn, 1);

  aLay->addWidget(aColorChoiceBtn, 0, 0);
  aLay->addWidget(aRandomChoiceBtn, 1, 0);

  myColorButton = new QtxColorButton(this);
  myColorButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  aLay->addWidget(myColorButton, 0, 1);

  QLabel* aRandomLabel = new QLabel(tr("Random"), this);
  aLay->addWidget(aRandomLabel, 1, 1);

  QDialogButtonBox* aButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                    Qt::Horizontal, this);
  connect(aButtons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), this, SLOT(reject()));
  aLay->addWidget(aButtons, 2, 0, 1, 2);
}

bool XGUI_ColorDialog::isRandomColor() const
{
  int anId = myButtonGroup->checkedId();

  return myButtonGroup->checkedId() == 1;
}

void XGUI_ColorDialog::setColor(const std::vector<int>& theValue)
{
  if (theValue.size() != 3)
    return;

  myColorButton->setColor(QColor(theValue[0], theValue[1], theValue[2]));
}

std::vector<int> XGUI_ColorDialog::getColor() const
{
  QColor aColorResult = myColorButton->color();

  std::vector<int> aValues;
  aValues.push_back(aColorResult.red());
  aValues.push_back(aColorResult.green());
  aValues.push_back(aColorResult.blue());

  return aValues;
}
// contains global cash for integer index of the color -> RGB of this color
static std::map<int, std::vector<int> > myColorMap;

void appendValues(std::vector<int>& theRGB, const int theRed, const int theGreen, const int theBlue)
{
  theRGB.push_back(theRed);
  theRGB.push_back(theGreen);
  theRGB.push_back(theBlue);
}

bool containsValues(std::map<int, std::vector<int> >& theColorMap, std::vector<int>& theValues)
{
  std::map<int, std::vector<int> >::const_iterator anIt = theColorMap.begin(),
                                                   aLast = theColorMap.end();
  bool isFound = false;
  for (; anIt != aLast && !isFound; anIt++) {
    std::vector<int> aValues = anIt->second;
    isFound = aValues[0] == theValues[0] &&
              aValues[1] == theValues[1] &&
              aValues[2] == theValues[2];
  }
  return isFound;
}

std::vector<int> HSVtoRGB(int theH, int theS, int theV)
{
  std::vector<int> aRGB;
  if (theH < 0 || theH > 360 ||
      theS < 0 || theS > 100 ||
      theV < 0 || theV > 100)
    return aRGB;

  int aHi = (int)theH/60;

  double aV = theV;
  double aVmin = (100 - theS)*theV/100;

  double anA = (theV - aVmin)* (theH % 60) / 60;

  double aVinc = aVmin + anA;
  double aVdec = theV - anA;

  double aPercentToValue = 255./100;
  int aV_int    = (int)(aV*aPercentToValue);
  int aVinc_int = (int)(aVinc*aPercentToValue);
  int aVmin_int = (int)(aVmin*aPercentToValue);
  int aVdec_int = (int)(aVdec*aPercentToValue);

  switch(aHi) {
    case 0: appendValues(aRGB, aV_int,    aVinc_int, aVmin_int); break;
    case 1: appendValues(aRGB, aVdec_int, aV_int,    aVmin_int); break;
    case 2: appendValues(aRGB, aVmin_int, aV_int,    aVinc_int); break;
    case 3: appendValues(aRGB, aVmin_int, aVdec_int, aV_int); break;
    case 4: appendValues(aRGB, aVinc_int, aVmin_int, aV_int); break;
    case 5: appendValues(aRGB, aV_int,    aVmin_int, aVdec_int); break;
    default: break;
  }
  return aRGB;
}


void fillColorMap()
{
  if (!myColorMap.empty())
    return;

  int i = 0;
  for (int s = 100; s > 0; s = s - 50)
  {
    for (int v = 100; v >= 40; v = v - 20)
    {
      for (int h = 0; h < 359 ; h = h + 60)
      {
        std::vector<int> aColor = HSVtoRGB(h, s, v);
        if (containsValues(myColorMap, aColor))
          continue;
        myColorMap[i] = aColor;
        i++;
      }
    }
  }
}

void findRandomColor(std::vector<int>& theValues)
{
  theValues.clear();
  if (myColorMap.empty()) {
    fillColorMap();
  }

  size_t aSize = myColorMap.size();
  int anIndex = rand() % aSize;
  if (myColorMap.find(anIndex) != myColorMap.end()) {
    theValues = myColorMap.at(anIndex);
  }
}

std::vector<int> XGUI_ColorDialog::getRandomColor() const
{
  std::vector<int> aValues;
  if (isRandomColor()) {
    findRandomColor(aValues);
  }
  return aValues;
}
