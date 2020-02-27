.. |import.icon|    image:: images/import.png

Import
======

The Import feature allows importing an external file into a current study.

Import PartSet
--------------

To open a study previously saved in SHAPER native format, select in the Main Menu *File -> Import -> Part Set...* item.
The current study will be closed, so the action dialog will be opened to choose whether the changes should be saved.
After that the following open file dialog will be opened:

.. image:: images/ImportPartFileDlg.png
   :align: center
	
.. centered::
   **Dialog box to open SHAPER native file**

Select file and press **Open** button to load the file. **Cancel** button cancels the operation.


Import Part
-----------

To import the previously stored file in the current part, select in the Main Menu *File -> Import -> Part...* item.

The following property panel will be opened:

.. image:: images/ImportPart_panel.png
   :align: center
	
.. centered::
   **Import Part property panel**

In this panel it is possible to enter a file name directly or press **'...'** button and browse it with help of import file dialog box:

.. image:: images/ImportPartFileDlg.png
   :align: center
	
.. centered::
   **Dialog box to import Part**

The import will be performed within the active part. If no part is active, the possible targets to import will be proposed in **Import to** combobox, which contains the list of existing parts, PartSet or a new part creation option.
  
**Apply** button imports the file.
  
**Cancel** button cancels the operation.

**TUI Command**:

.. py:function:: model.importPart(Doc, FileNameString, [PrevFeature])

    :param part: The current part object
    :param string: A file name string
    :param reference: The feature after which the imported entities should be inserted


Import from CAD formats
-----------------------

It is possible to import files in the following CAD-neutral formats: BREP, STEP, IGES, XAO.
To import a file into active part select in the Main Menu *File -> Import -> From CAD format...* item.

The following property panel will be opened:

.. image:: images/Import_panel.png
   :align: center
	
.. centered::
   **Import property panel**

The **Import to** combobox provides the list of destinations (PartSet, existing Part or a new Part).

In this panel it is possible to enter a file name directly or press **'...'** button and browse it with help of import file dialog box:

.. image:: images/OpenFileDlg.png
   :align: center
	
.. centered::
   **Dialog box to import CAD-neutral format**
  
**Apply** button imports the file.
  
**Cancel** button cancels the operation.

**TUI Command**:

.. py:function:: model.addImport(Part_doc, FileNameString)

    :param part: The current part object
    :param string: A file name string.

Result
""""""

The Result of the operation depends on the imported file content.

.. image:: images/FileImported.png
	   :align: center
		   
.. centered::
   Import of BREP file.

**See Also** a sample TUI Script of :ref:`tui_import_file` operation.
