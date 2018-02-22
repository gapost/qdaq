############# linux 3rd party libs ##############

unix {
    ########## system libs ##########
    LIBS += -lrt -lpthread

    ########## ATLAS ################
    centos6 { LIBS += -lcblas -latlas -lm }
    else { LIBS += -lm }

    ############## GSL ##############
    LIBS += -lgsl

    ############## HDF5 ##############
    LIBS += -lhdf5 -lhdf5_cpp

    ############## muParser ##############
    LIBS += -lmuparser

    ######### libmodbus #############
    LIBS += -lmodbus
    INCLUDEPATH += /usr/include/modbus

    ######### comedi #############
    LIBS += -lcomedi

    ######### Qwt ###############
    INCLUDEPATH += /usr/include/qt5/qwt
    LIBS += -lqwt-qt5

    ######### QtSolutions ###############
    INCLUDEPATH += /usr/include/qt5/QtSolutions
    LIBS += -lQtSolutions_PropertyBrowser
}



