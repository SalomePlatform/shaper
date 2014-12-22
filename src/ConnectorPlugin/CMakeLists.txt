INCLUDE(Common)

SET(PYTHON_FILES
    ConnectorPlugin.py
    ConnectorPlugin_ExportFeature.py
)

SET(XML_RESSOURCES
  plugin-Connector.xml
)

ADD_CUSTOM_TARGET(ConnectorPlugin SOURCES ${PYTHON_FILES} ${XML_RESSOURCES})

INSTALL(FILES ${PYTHON_FILES} ${XML_RESSOURCES} DESTINATION plugins)
