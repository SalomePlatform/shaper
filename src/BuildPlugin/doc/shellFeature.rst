
Shell
=====

The Shell feature creates a shell using already existing faces in other objects.

To create shell in the active part:

#. select in the Main Menu *Build - > Shell* item  or
#. click **Shell** button in the toolbar

.. image:: images/feature_shell.png
  :align: center

.. centered::
  **Shell** button

The following property panel will be opened:

.. image:: images/Shell.png
  :align: center

.. centered::
  Create a shell
  
Select one or several faces in viewer.

**Apply** button creates a shell.

**Cancel** button cancels operation. 

**TUI Command**:  *model.addShell(Part_doc, Objects)*

**Arguments**:   Part document + list of shapes.

Result
""""""

The result of the operation will be shell created from selected shapes:

.. image:: images/CreateShell.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of a :ref:`tui_create_shell` operation.
