// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "PartSet_DataTreeModel.h"

PartSet_FeaturesModel::PartSet_FeaturesModel(QObject* theParent) 
: QAbstractItemModel(theParent),
  myItemsColor(Qt::black)
{

}

PartSet_PartModel::PartSet_PartModel(QObject* theParent)
: PartSet_FeaturesModel(theParent)
{

}

PartSet_PartModel::~PartSet_PartModel()
{
  myPart = FeaturePtr();
}
