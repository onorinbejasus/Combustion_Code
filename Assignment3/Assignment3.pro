#-------------------------------------------------
#
# Project created by QtCreator 2012-03-15T13:16:30
#
#-------------------------------------------------

QT       += core gui opengl -std=c++0x

TARGET = Assignment3
TEMPLATE = app

SOURCES +=\
    main.cc \
    RotTools.cc \
    fileOps.cc \
    AllocArray.cpp \
    Arrows.cpp \
    box.cpp \
    callbacks.cpp \
    ColorTransferFunction.cpp \
    FatalError.cpp \
    glwidget.cpp \
    mainwindow.cpp \
    MathTools.cpp \
    Parameters.cpp \
    point.cpp \
    Projector.cpp \
    Shaders.cpp \
    Streamline.cpp \
    SuperQuadrics.cpp \
    TransferFunction.cpp \
    Transform.cpp \
    vbo.cpp \
    Vector.cpp \
    Vector3.cpp \
    Volume.cpp \
    VolumeRenderer.cpp

HEADERS  += \
    mainwindow.hh \
    enums.hh \
    VolumeRenderer.hh \
    Volume.hh \
    VectorFieldRenderer.hh \
    Vector3.hh \
    Vector.hh \
    Transform.hh \
    TransferFunction.hh \
    Streamline.hh \
    Shaders.h \
    Projector.hh \
    Point3.hh \
    point.hh \
    Parameters.hh \
    MathTools.hh \
    FatalError.hh \
    ColorTransferFunction.hh \
    AllocArray.hh \
    open_gl.hh \
    glwidget.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    Assignment3.pro.user
