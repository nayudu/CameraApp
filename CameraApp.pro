
QT       += core gui multimediawidgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CameraApp
TEMPLATE = app


SOURCES += main.cpp\
            AppUI.cpp\
            AppController.cpp\
            CameraWidget.cpp \
            xiApiPlusOcv.cpp \
            CanvasWidget.cpp \
    SerialWorker.cpp

HEADERS  += AppUI.h\
            AppController.h\
            CameraWidget.h\
            package/include/\
            xiApiPlusOcv.hpp \
            CanvasWidget.h \
    SerialWorker.h \
    Utils.h

CONFIG += c++11

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/package/libs/libraw1394/X64/release/ -lraw1394
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/package/libs/libraw1394/X64/debug/ -lraw1394
else:unix: LIBS += -L$$PWD/package/libs/libraw1394/X64/ -lraw1394

INCLUDEPATH += $$PWD/package/libs/libraw1394/X64
DEPENDPATH += $$PWD/package/libs/libraw1394/X64

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/package/libs/libusb/X64/release/ -lusb-1.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/package/libs/libusb/X64/debug/ -lusb-1.0
else:unix: LIBS += -L$$PWD/package/libs/libusb/X64/ -lusb-1.0

INCLUDEPATH += $$PWD/package/libs/libusb/X64
DEPENDPATH += $$PWD/package/libs/libusb/X64

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/release/ -lm3api
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/debug/ -lm3api
else:unix: LIBS += -L$$PWD/../../../../../../../usr/lib/ -lm3api

INCLUDEPATH += C:\opencv\build\include
INCLUDEPATH += C:\OpenCVGraph\Capture\CameraSDKs\Ximea\API

LIBS += C:\opencv_build\bin\libopencv_core320.dll
LIBS += C:\opencv_build\bin\libopencv_highgui320.dll
LIBS += C:\opencv_build\bin\libopencv_imgcodecs320.dll
LIBS += C:\opencv_build\bin\libopencv_imgproc320.dll
LIBS += C:\opencv_build\bin\libopencv_features2d320.dll
LIBS += C:\opencv_build\bin\libopencv_calib3d320.dll
LIBS +=C:\OpenCVGraph\Capture\CameraSDKs\Ximea\API\x64\m3apiX64.lib
LIBS +=C:\OpenCVGraph\Capture\CameraSDKs\Ximea\API\x64\xiapi64.lib
FORMS +=


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opencv/build/x64/vc15/lib/ -lopencv_world410
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opencv/build/x64/vc15/lib/ -lopencv_world410d
else:unix: LIBS += -L$$PWD/../../../../opencv/build/x64/vc15/lib/ -lopencv_world410

INCLUDEPATH += $$PWD/../../../../opencv/build/include/opencv2
DEPENDPATH += $$PWD/../../../../opencv/build/include/opencv2
