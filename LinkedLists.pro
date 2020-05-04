TEMPLATE = subdirs

SUBDIRS += \
    LinkedListsLib \
    Utils \
    Tests \
    Applications

Applications.depends = LinkedListsLib
Applications.depends = Utils
LinkedListsLib.depends = Utils
Tests.depends = LinkedListsLib
Tests.depends = Utils
