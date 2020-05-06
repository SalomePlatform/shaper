.. |partition_btn.icon|    image:: images/partition_btn.png

Partition
=========

Partition feature implements a boolean operation for partitioning of a set of selected objects.

To perform a Partition in the active part:

#. select in the Main Menu *Features - > Partition* item  or
#. click |partition_btn.icon| **Partition** button in the toolbar

The following property panel will be opened:

.. figure:: images/Partition.png
   :align: center

   **Partition operation**

**Base Objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be partitioned.

**TUI Command**:

.. py:function:: model.addPartition(Part_doc, objects)

    :param part: The current part object.
    :param object: A list of objects.
    :return: Created object.

Result
""""""

The Result of the operation will be a shape which is a partition of selected objects:

.. figure:: images/CreatedPartition.png
   :align: center

   **Partition created**

**See Also** a sample TUI Script of :ref:`tui_create_partition` operation.
