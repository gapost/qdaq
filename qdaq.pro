################################################################
#
# QDaq - Qt-based data aqcuisition
#
# Sub-Project Description
#
################################################################
#
# 1. core
#
# Builds the QDaqCore shared library (libQDaqCore.so or QDaqCore.dll). 
#
# 2. gui
#
# Builds the QDaqGui shared library (libQDaqGui.so or QDaqGui.dll).
#
# 3. qdaq
#
# Builds the qdaq application, which can be used to run qdaq-javascript scripts.
#
# 3. designer
#
# Builds the QtDesigner plugins for some of the widgets in QDaqGui.
#
# 4. filters
#
# Builds the QDaqFilters shared library (libQDaqFilters.so or QDaqFilters.dll).
#
# 5. interfaces
#
# Builds the QDaqInterfaces shared library (libQDaqInterfaces.so or QDaqInterfaces.dll).
#
# 6. docs
#
# Dummy project, contains the doxygen config file and documentation
#
# 7. test
#
# Dummy project, contains test qdaq scripts for testing and debugging.
#
# 8. plugins
#
# Project to build qdaq javascript plugins
#
###################################################################

TEMPLATE = subdirs
CONFIG += ordered


SUBDIRS += \
    core \
    gui \
    qdaq \
    designer \
    filters \
    interfaces \
    docs \
    test \
    plugins

qdaqspec.files  = \
    qdaq-core.prf \
    qdaq-gui.prf \
    qdaq-filters.prf \
    qdaq-interfaces.prf

qdaqspec.path  = $$[QMAKE_MKSPECS]/features

INSTALLS += qdaqspec

