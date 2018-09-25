
Import
======

The feature Import allows to import an external file into a current study.

It is possible to import files in following formats: BREP, STEP, IGES, XAO. To import a file into active part:

#. select in the Main Menu *Part - > Import* item  or
#. click **Import** button in the toolbar.

.. image:: images/import.png
   :align: center

.. centered::
   **Import**  button 

The following property panel will be opened:

.. image:: images/Import_panel.png
   :align: center
	
.. centered::
   **Import property panel**

In this panel it is possible to enter a file name directly into input field or press **'...'** button and browse it with help of import file dialog box:

.. image:: images/OpenFileDlg.png
   :align: center
	
.. centered::
   **Import file dialog box**
  
**Apply** button imports the file.
  
**Cancel** button cancels operation.

**TUI Command**: *model.addImport(Part_doc, FileNameString)*

**Arguments**: Part + file name string.

Result
""""""

The Result of the operation depends on content of imported file.

.. image:: images/FileImported.png
	   :align: center
		   
.. centered::
   Import of BREP file.

**See Also** a sample TUI Script of a :ref:`tui_import_file` operation.
