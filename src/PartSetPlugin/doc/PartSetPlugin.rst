.. |new_part.icon|    image:: images/new_part.png
.. |remove.icon|    image:: images/remove.png
.. |duplicate.icon|    image:: images/duplicate.png

.. _partPlugin:

Part plug-in
============

.. _new-part:

New Part
--------

To create a New Part:

#. select in the Main Menu *Part - > New part* item  or
#. click |new_part.icon| **New part** button in Shaper toolbar:

**TUI Command**:

.. py:function:: model.addPart(partSet)

    :param part: The current part object.
    :return: Result object.

Result
""""""

Created empty part is activated and appears in the object browser.

**See Also** a sample TUI Script of :ref:`tui_create_part` operation.

Duplicate Part
--------------

To duplicate an active Part:

#. select in the Main Menu *Part - > Duplicate part* item  or
#. click |duplicate.icon| **Duplicate part** button in Shaper toolbar:

Result
""""""

Created copied part is activated and appears in the object browser.

Copied part contains all objects existing in the source part.

Remove Part
-----------

To remove an active Part:

#. select in the Main Menu *Part - > Remove part* item  or
#. click |remove.icon| **Remove part** button in Shaper toolbar:

Result
""""""

Selected part is removed together with all its objects.

