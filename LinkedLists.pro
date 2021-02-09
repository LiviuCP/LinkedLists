TEMPLATE = subdirs

SUBDIRS += \
    LinkedListsLib \
    Utils \
    Tests \
    Applications \
    Collections

LinkedListsLib.depends = Utils
Collections.depends = LinkedListsLib
Applications.depends = LinkedListsLib
Tests.depends = Collections
