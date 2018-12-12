#!/bin/bash -ex

source env.sh

cd ${BUILD_DIR}

lcov --capture --directory ${BUILD_DIR} --no-external --base-directory ${SOURCES_DIR} --output-file coverage.info.noext -q

# make a working copy of report
cp -f coverage.info.noext covfile
# remove all reports of GUI and external parts (for all the next kinds of reports)
# RefAttrList is unused type of attribute for now
for MASK in '*wrap*' 'moc_*' 'XAO_*' 'SketcherPrs_*' 'GeomAlgoImpl_*' 'ModuleBase_*' '*Widget*' '*Splitter*' '*RefAttrList*'; do
lcov -r covfile ${MASK} --output-file covfile_res -q
mv -f covfile_res covfile
done

ALL='BuildPlugin CollectionPlugin ConstructionPlugin ExchangePlugin FeaturesPlugin GDMLPlugin PrimitivesPlugin InitializationPlugin ParametersPlugin PartSetPlugin SketchPlugin'
ALL+=' GDMLAPI PrimitivesAPI BuilderAPI CollectionAPI ConnectorAPI ConstructionAPI ModelAPI ExchangeAPI FeaturesAPI ModelHighAPI ParametersAPI PartSetAPI SketchAPI BuildAPI GeomDataAPI GeomAPI GeomAlgoAPI'
ALL+=' Config Events GeomValidators Model_ ModelGeomAlgo Selector SketchSolver GeomData'

# prepare API report
cp -f covfile covAPI
# remove all plugins data except the needed
NEED='BuildAPI CollectionAPI ConnectorAPI ConstructionAPI ExchangeAPI FeaturesAPI GDMLAPI ModelHighAPI ParametersAPI PartSetAPI PrimitivesAPI SketchAPI'
for MASK in $ALL; do
  if ! [[ " $NEED " =~ " $MASK " ]]; then
    lcov -r covAPI *${MASK}* --output-file covAPI_res -q
    mv -f covAPI_res covAPI
  fi
done
rm -rf lcov_htmlAPI
genhtml covAPI --output-directory lcov_htmlAPI -q


# prepare Direct report
cp -f covfile covDirect
# remove all plugins data except the needed
NEED='GeomAlgoAPI GeomAPI'
for MASK in $ALL; do
  if ! [[ " $NEED " =~ " $MASK " ]]; then
    lcov -r covDirect *${MASK}* --output-file covDirect_res -q
    mv -f covDirect_res covDirect
  fi
done
rm -rf lcov_htmlDirect
genhtml covDirect --output-directory lcov_htmlDirect -q


# prepare Else report
cp -f covfile covElse
# remove all plugins data except the needed
NEED='BuildPlugin CollectionPlugin Config ConstructionPlugin Events ExchangePlugin FeaturesPlugin GDMLPlugin GeomData GeomDataAPI GeomValidators InitializationPlugin Model_ ModelAPI ModelGeomAlgo ParametersPlugin PartSetPlugin PrimitivesPlugin Selector SketchPlugin SketchSolver'
for MASK in $ALL; do
  if ! [[ " $NEED " =~ " $MASK " ]]; then
    lcov -r covElse *${MASK}* --output-file covElse_res -q
    mv -f covElse_res covElse
  fi
done
rm -rf lcov_htmlElse
genhtml covElse --output-directory lcov_htmlElse -q

# go back
cd ${SOURCES_DIR}
