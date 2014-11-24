INCLUDE(Common)

SET(PYTHON_FILES
  PythonFeaturesPlugin_Box.py
  PythonFeaturesPlugin.py
  sketch.py
  extrusion.py
  examples.py
  SketchResult.py
)

SET(XML_RESSOURCES
  plugin-PythonFeatures.xml
  box_widget.xml
)

INSTALL(FILES ${PYTHON_FILES} ${XML_RESSOURCES} DESTINATION plugins)
