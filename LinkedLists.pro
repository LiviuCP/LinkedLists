TEMPLATE = subdirs

SUBDIRS += \
    Application \
    LinkedListsLib \
    Utils

Application.depends = LinkedListsLib
Application.depends = Utils
LinkedListsLib.depends = Utils
