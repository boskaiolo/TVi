######################################################################
# Automatically generated by qmake (2.01a) Thu Jul 1 12:06:17 2010
######################################################################

QT = core gui opengl sql
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . /Users/bosko/Develop/boost_1_46_1 /Users/bosko/Develop/OGDF


LIBS += -glew -glxew -wglew -glut -framework OpenGL -framework GLUT -L/Users/bosko/Develop/OGDF/_release_O1/ -lOGDF
#LIBS += -glew -glxew -wglew -glut -framework OpenGL -framework GLUT -L/Users/bosko/Develop/coin-Osi/lib -L/Users/bosko/Develop/coin-Clp/lib -lCoinUtils -lOsi -lOsiClp -lClp -L/Users/bosko/Develop/OGDF/_release_fast_coin_clp/ -lOGDF

# Input
HEADERS += featuresbox.h \
    entropy_viewer.h \
    colors.h \
    db_credential.h \
    histogram_viewer.h \
    tools.h \
    rowstable.h \
    pixel2gl.h \
    anomaly_stuff.h \
    well_known_ports.h \
    graphvisualizer.h \
    graphfeatureselector.h \
    main.h \
    matrix_features.h \
    matrix_visualizer.h \
    timeforwardbackward.h \
    timestamp_selected.h \
    hist_tab.h
SOURCES += featuresbox.cpp main.cpp \
    entropy_viewer.cpp \
    histogram_viewer.cpp \
    tools.cpp \
    rowstable.cpp \
    pixel2gl.cpp \
    anomaly_stuff.cpp \
    well_known_ports.cpp \
    graphvisualizer.cpp \
    graphfeatureselector.cpp \
    matrix_features.cpp \
    matrix_visualizer.cpp \
    timeforwardbackward.cpp \
    hist_tab.cpp

OTHER_FILES +=

RESOURCES += \
    images.qrc

RC_FILE = icon.icns
