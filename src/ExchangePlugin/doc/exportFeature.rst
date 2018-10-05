
Export
======

The  export feature allows exporting objects from the current study to an external file.

It is possible to export objects to files in the following formats: BREP, STEP, IGES, XAO. To export objects to a file:

#. select in the Main Menu *Part - > Export* item  or
#. click **Export** button in the toolbar.

.. image:: images/export.png
   :align: center

.. centered::
   **Export**  button

The following property panel will be opened:

.. image:: images/Export_panel.png
   :align: center
	
.. centered::
   **Export property panel**

In this panel, it is necessary to select desirable format of export file. It can be **'BREP, STEP, IGES'** or **'XAO'**. In case of first choice the format of exported file will be defined according to file extension. The file name and path can be defined in **Export file** field by direct input or browsing with **'...'** button, which opens **Export file** dialog box:

.. image:: images/ExportFileDlg.png
   :align: center
	
.. centered::
   **Export file dialog box**

Selection list in the property panel contains a list of exported objects which can be selected in a viewer or object browser.

**Apply** button exports the file.
  
**Cancel** button cancels the operation.

**TUI Command**: *model.exportToFile(Part_doc, FileNameString, ObjectsList)*

**Arguments**: Part + file name string + list of objects.

Result
""""""

The Result of operation is an exported file.

**See Also** a sample TUI Script of :ref:`tui_export_file` operation.
