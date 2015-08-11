TEMPLATE = subdirs

SUBDIRS += \
    linalg \
    strain \
    strainui \
    straincli \
    strainGuiApp

strain.depends = linalg
strainui.depends = strain
strincli.depends = strainui
strainGuiApp.depends = strainui
