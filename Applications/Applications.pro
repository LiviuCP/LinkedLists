TEMPLATE = subdirs

SUBDIRS += \
    ManualListEntry

unix {
    SUBDIRS += \
        BoundedBuffer \
        UnnamedPipes \
        NamedPipes \
        MessageQueues \
        Sockets \
        SharedFile \
        SharedMemory \
        ASyncTest \
        FuturePromiseTest \
        Signals
}
