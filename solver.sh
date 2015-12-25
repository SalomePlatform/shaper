#!/bin/bash
#
# Modify plugins.xml to switch solver

export PLUGINS_PATH=${PLUGINS_PATH:-${NEWGEOM_ROOT_DIR}/plugins/plugins.xml}

while [[ $# > 0 ]]; do
  key="$1"
  case $key in
    solvespace|ss)
      export solver=SolveSpace
      shift
      ;;
    planegcs|gcs)
      export solver=PlaneGCS
      shift
      ;;
    -p)
      export PLUGINS_PATH="$2" 
      shift
      shift
      ;;
    *)
      shift
      ;;
  esac
done

if [[ -z ${solver:-} ]]; then exit 0; fi

case $solver in
  PlaneGCS)
    export SWITCH_ON=PlaneGCSSolver
    export SWITCH_OFF=SolveSpaceSolver
    ;;
  SolveSpace|*)
    export SWITCH_ON=SolveSpaceSolver
    export SWITCH_OFF=PlaneGCSSolver
    ;;
esac

echo "Switch $solver solver in ${PLUGINS_PATH}"

cat ${PLUGINS_PATH} \
  | tr '\n' '\r' \
  | sed -e "s|\r  <plugin library=\"[^\r]*Solver\"\/>\r<!--|\r  <plugin library=\"${solver}Solver\"\/>\r<!--|" \
  | tr '\r' '\n' \
  > ${PLUGINS_PATH}_out
mv -f ${PLUGINS_PATH}_out ${PLUGINS_PATH}
cat ${PLUGINS_PATH}

