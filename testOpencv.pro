
HEADERS       = quadsegmentation.h

SOURCES       = main.cpp \
                quadsegmentation.cpp

# OpenCV support

INCLUDEPATH = e:\download\browser\opencv\build\include\

OPENCV_LIBS = -lopencv_core -lopencv_highgui -lopencv_objdetect
win32 {
  win32-msvc* {
    CONFIG(debug, debug|release): OPENCV_LIBS = -lopencv_core2410d -lopencv_highgui2410d -lopencv_objdetect2410d -lopencv_imgproc2410d
    LIBS += -L"e:\download\browser\opencv\build\x86\vc12\lib"
  } else: {
    OPENCV_LIBS = -lopencv_core247 -lopencv_highgui247 -lopencv_objdetect247
    LIBS += -L"$$PWD/libs/mingw"
  }
}

LIBS += $$OPENCV_LIBS
