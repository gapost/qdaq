################################################################
#
# QDaq - Qt-based data aqcuisition
#
# plugins/ Sub-Project Description
#
################################################################
#
# 1. designer
#
# Builds the QtDesigner plugins for some of the widgets in QDaqGui.
#
# 2. filters-plugin
#
# Builds a QtScript plugin/extention to load QDaqFilters into a QtScript environment 
#
# 3. interfaces-plugin
#
# Builds a QtScript plugin/extention to load QDaqInterfaces into a QtScript environment 
#
###################################################################

TEMPLATE = subdirs

SUBDIRS += \
    designer \
    filters-plugin \
    interfaces-plugin
