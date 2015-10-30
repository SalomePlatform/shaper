"""Import Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addImport(part, *args):
    """Add an Import feature to the Part and return Import.

    Pass all args to Import __init__ function.
    """
    feature = part.addFeature("Import")
    return Import(feature, *args)


class Import(Interface):
    """Interface on an Import feature."""

    def __init__(self, feature, *args):
        """Initialize an Import feature with given parameters.

        Expected arguments for all modes:
        feature -- an Import feature

        For initialization (expect 1 argument):
        file_path -- path to the imported file
        """
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Import")

        self._file_path = self._feature.data().string("file_path")

        assert(self._file_path)

        if not args:
            return

        assert(len(args) == 1)
        self.setFilePath(args[0])

        self._execute()
        pass

    def setFilePath(self, file_path):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._file_path, file_path)
        pass


def exportToFile(part, *args):
    """Add an Export feature to the Part and return Export.

    Pass all args to Export __init__ function.
    """
    feature = part.addFeature("Export")
    return Export(feature, *args)


class Export(Interface):
    """Interface on an Export feature."""

    def __init__(self, feature, *args):
        """Initialize an Export feature with given parameters.

        Expected arguments for all modes:
        feature -- an Export feature

        For initialization (expect 3 argument):
        file_path -- path to the exported file
        file_format -- format of to the exported file
        objects -- objects to export
        """
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Export")

        self._file_path = self._feature.data().string("file_path")
        self._file_format = self._feature.data().string("file_format")
        self._objects = self._feature.data().selectionList("selection_list")

        assert(self._file_path)
        assert(self._file_format)
        assert(self._objects)

        if not args:
            return

        assert(len(args) == 3)
        self.setFilePath(args[0])
        self.setFileFormat(args[1])
        self.setObjects(args[2])

        self._execute()
        pass

    def setFilePath(self, file_path):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._file_path, file_path)
        pass

    def setFileFormat(self, file_format):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._file_format, file_format)
        pass

    def setObjects(self, objects):
        """Modify file path attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._objects, object)
        pass
