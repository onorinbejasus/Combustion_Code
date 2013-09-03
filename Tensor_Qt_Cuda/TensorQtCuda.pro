#-------------------------------------------------
#
# Project created by QtCreator 2012-03-15T13:16:30
#
#-------------------------------------------------

QT       += core gui opengl -std=c++0x
CONFIG += console
CONFIG -= app_bundle
TARGET = TensorQtCuda
TEMPLATE = app

# Setup your CUDA & sources
include(cuda.prf)
add_cuda_source(helloworld.cu)
add_cuda_source(reductionMax.cu)
add_cuda_source(sharedMem.cuh)
add_cuda_source(volumeRender_kernel.cuh)
add_cuda_source(volumeRender_kernel.cu)

SOURCES +=\
    main.cc \
    RotTools.cc \
    fileOps.cc \
    AllocArray.cpp \
    Arrows.cpp \
    box.cpp \
    callbacks.cpp \
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
    Transform.cpp \
    vbo.cpp \
    Vector.cpp \
    Vector3.cpp \
    Volume.cpp

HEADERS  += \
    mainwindow.hh \
    enums.hh \
    Volume.hh \
    VectorFieldRenderer.hh \
    Vector3.hh \
    Vector.hh \
    Transform.hh \
    Streamline.hh \
    Shaders.h \
    Projector.hh \
    Point3.hh \
    point.hh \
    Parameters.hh \
    MathTools.hh \
    FatalError.hh \
    AllocArray.hh \
    open_gl.hh \
    glwidget.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    TensorQtCuda.pro.user \
    cuda.prf

mac: LIBS += -framework GLUT
else:unix|win32: LIBS += -lGLUT
