################################################################
#
# QDaq - Qt-based data aqcuisition
#
# src/ Sub-Project Description
#
################################################################
#
# 1. src/core
#
# Builds the QDaqCore shared library (libQDaqCore.so or QDaqCore.dll). 
#
# 2. src/gui
#
# Builds the QDaqGui shared library (libQDaqGui.so or QDaqGui.dll).
#
# 3. filters
#
# Builds the QDaqFilters shared library (libQDaqFilters.so or QDaqFilters.dll).
#
# 4. interfaces
#
# Builds the QDaqInterfaces shared library (libQDaqInterfaces.so or QDaqInterfaces.dll).
#
#
###################################################################

TEMPLATE = subdirs
CONFIG += ordered


SUBDIRS += \
    core \
    gui \
    filters \
    interfaces 
