TEMPLATE = subdirs

SUBDIRS += \
    Application \
    LinkedListsLib \
    Utils \
    Tests

Application.depends = LinkedListsLib
Application.depends = Utils
LinkedListsLib.depends = Utils
Tests.depends = LinkedListsLib
Tests.depends = Utils
