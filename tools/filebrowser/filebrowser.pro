QT += widgets
# requires(qtConfig(filedialog))

TARGET = filebrowser
TEMPLATE = app
DESTDIR = $$OUT_PWD/../../bin

HEADERS       = mainwindow.h \
                mdichild.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                mdichild.cpp
RESOURCES     = mdi.qrc

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/mdi
# INSTALLS += target
