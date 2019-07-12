.. |feature_shell.icon|    image:: images/feature_shell.png

Shell
=====

Shell feature creates a shell using faces already existing in other objects.

To create a shell in the active part:

#. select in the Main Menu *Build - > Shell* item  or
#. click |feature_shell.icon| **Shell** button in the toolbar

The following property panel will be opened:

.. image:: images/Shell.png
  :align: center

.. centered::
  Create a shell
  
Select one or several faces in the viewer.

**Apply** button creates a shell.

**Cancel** button cancels the operation. 

**TUI Command**:

.. py:function:: model.addShell(Part_doc, Objects)

    :param part: The current part object.
    :param list: A list of shapes.
    :return: Result object.

Result
""""""

The result of the operation will be a shell created from the selected shapes:

.. image:: images/CreateShell.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_shell` operation.
