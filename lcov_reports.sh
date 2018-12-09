#!/bin/bash -ex

source env.sh

cd ${BUILD_DIR}

lcov --capture --directory ${BUILD_DIR} --no-external --base-directory ${SOURCES_DIR} --output-file coverage.info.noext -q

# make a working copy of report
cp -f coverage.info.noext covfile
# remove all reports of GUI and external parts (for all the next kinds of reports)
for MASK in '*wrap*' 'moc_*' 'XAO_*' 'SketcherPrs_*' 'GeomAlgoImpl_*' 'ModuleBase_*' '*Widget*' '*Splitter*'; do
lcov -r covfile ${MASK} --output-file covfile_res -q
mv -f covfile_res covfile
done


# prepare API report
cp -f covfile covAPI
# remove plugins
for MASK in 'Build' 'Collection' 'Construction' 'Exchange' 'Features' 'GDML' 'Primitives' 'Initialization' 'Parameters' 'PartSet' 'Sketch'; do
lcov -r covAPI *${MASK}Plugin* --output-file covAPI_res -q
mv -f covAPI_res covAPI
done
# remove low level API
for MASK in 'Geom' 'GeomAlgo' 'GeomData' 'Model'; do
lcov -r covAPI *${MASK}API* --output-file covAPI_res -q
mv -f covAPI_res covAPI
done
# remove others
for MASK in 'Config' 'Events' 'GeomData' 'GeomValidators' 'Model_' 'ModelGeomAlgo' 'Selector' 'SketchSolver'; do
lcov -r covAPI *${MASK}* --output-file covAPI_res -q
mv -f covAPI_res covAPI
done
rm -rf lcov_htmlAPI
genhtml covAPI --output-directory lcov_htmlAPI -q


# prepare Direct report
cp -f covfile covDirect
# remove plugins
for MASK in 'Build' 'Collection' 'Construction' 'Exchange' 'Features' 'GDML' 'Primitives' 'Initialization' 'Parameters' 'PartSet' 'Sketch'; do
str=$startmask$MASK$endmask
lcov -r covDirect *${MASK}Plugin* --output-file covDirect_res -q
mv -f covDirect_res covDirect
done
# remove low level API
for MASK in 'GDML' 'Primitives' 'Builder' 'Collection' 'Connector' 'Construction' 'Model' 'Exchange' 'Features' 'ModelHigh' 'Parameters' 'PartSet' 'Sketch' 'Build' 'GeomData'; do
lcov -r covDirect *${MASK}API* --output-file covDirect_res -q
mv -f covDirect_res covDirect
done
# remove others
for MASK in 'Config' 'Events' 'GeomValidators' 'Model_' 'ModelGeomAlgo' 'Selector' 'SketchSolver' 'GeomData'; do
lcov -r covDirect *${MASK}* --output-file covDirect_res -q
mv -f covDirect_res covDirect
done
rm -rf lcov_htmlDirect
genhtml covDirect --output-directory lcov_htmlDirect -q


# prepare Else report
cp -f covfile covElse
# remove low level API
for MASK in 'Geom' 'GeomAlgo' 'GDML' 'Primitives' 'Builder' 'Collection' 'Connector' 'Construction' 'Exchange' 'Features' 'ModelHigh' 'Parameters' 'PartSet' 'Sketch' 'Build'; do
lcov -r covElse *${MASK}API* --output-file covElse_res -q
mv -f covElse_res covElse
done
# remove SketchPlugin's Ellipse feature (unsupported yet)
lcov -r covElse SketchPlugin*Ellipse* --output-file covElse_res -q
mv -f covElse_res covElse
# remove GUI related files from Config plugin
for MASK in 'DataModelReader' 'Translator' 'PointerMessage'; do
lcov -r covElse *Config_${MASK}* --output-file covElse_res -q
mv -f covElse_res covElse
done
rm -rf lcov_htmlElse
genhtml covElse --output-directory lcov_htmlElse -q


# go back
cd ${SOURCES_DIR}
