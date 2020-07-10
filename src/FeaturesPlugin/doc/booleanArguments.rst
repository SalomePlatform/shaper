.. |common.icon|    image:: images/bool_common.png
   :height: 16px
.. |cut.icon|    image:: images/bool_cut.png
   :height: 16px
.. |fuse.icon|    image:: images/bool_fuse.png
   :height: 16px
.. |smash.icon|    image:: images/bool_smash.png
   :height: 16px
.. |split.icon|    image:: images/bool_split.png
   :height: 16px
.. |partition.icon|    image:: images/partition_btn.png
   :height: 16px
.. |inter.icon|    image:: images/intersection_btn.png
   :height: 16px
.. |union.icon|    image:: images/union_btn.png
   :height: 16px


.. _bopArguments:

Arguments of Boolean Operations
===============================

This section describes the shapes applicable as arguments of Boolean Operations.
First of all, the result of Boolean Operations depends on the dimension of arguments. The following table shows the dimension and corresponding types of shapes.

+-----------+----------------------------+
| Dimension | Shape types                |
+===========+============================+
|     0     | VERTEX, COMPOUND           |
+-----------+----------------------------+
|     1     | EDGE, WIRE, COMPOUND       |
+-----------+----------------------------+
|     2     | FACE, SHELL, COMPOUND      |
+-----------+----------------------------+
|     3     | SOLID, COMPSOLID, COMPOUND |
+-----------+----------------------------+

*Note*: COMPOUND may consist of any shape combinations, therefore, it is present is each row.

The next table aligns an operation and the dimensions of its arguments.
Construction planes (mentioned PLANE) can be used in several operations, but not included in the table above due to their infinite nature. 

+-------------------------+---------------------------+-------------------------+
|      Operation          | Dimension of objects (DO) | Dimension of tools (DT) |
+=========================+===========================+=========================+
| |common.icon|           |            Any            |      Any (+ PLANE)      |
| :ref:`featureCommon`    |                           |                         |
+-------------------------+---------------------------+-------------------------+
| |cut.icon|              |            Any            |  Not less than **DO**   |
| :ref:`featureCut`       |                           |                         |
+-------------------------+---------------------------+-------------------------+
| |fuse.icon|             |            Any            |          Any            |
| :ref:`featureFuse`      |                           |                         |
+-------------------------+---------------------------+-------------------------+
| |inter.icon|            |          1 or 2           |          ---            |
| :ref:`featureIntersect` |                           |                         |
+-------------------------+---------------------------+-------------------------+
| |partition.icon|        |       Any (+ PLANE)       |          ---            |
| :ref:`featurePartition` |                           |                         |
+-------------------------+---------------------------+-------------------------+
| |smash.icon|            | 2 (planar FACE only) or 3 |    Equal to **DO**      |
| :ref:`featureSmash`     |                           |                         |
+-------------------------+---------------------------+-------------------------+
| |split.icon|            |            Any            |      Any (+ PLANE)      |
| :ref:`featureSplit`     |                           |                         |
+-------------------------+---------------------------+-------------------------+
| |union.icon|            | 3 (SOLIDs from COMPSOLID) |          ---            |
| :ref:`featureUnion`     |                           |                         |
+-------------------------+---------------------------+-------------------------+
