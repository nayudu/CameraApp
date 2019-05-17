#ifndef AppController_H
#define AppController_H

#include <QObject>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <map>

class CameraWidget;
class AppUI;

class QByteArray;
class QDir;
class QImage;

/*
  This class will control the AppUI.
  This class will also act as an interface between UI and Network layer
*/
class AppController : public QObject
{
  Q_OBJECT
public:
    AppController( AppUI& inMainWindow );
private:
    void ProcessImage( cv::Mat const & inImage );
private:
    AppUI & mMainWindow;
    CameraWidget* mCameraWidget;

    // enable saving to disk
    bool mEnableSave;
    QDir* mDir;

    QString mImageNamePrefix;
    int mCounter;
    int mYCounter;

    cv::Mat mImg;
    std::map<std::string, cv::Mat > mImagesToSave;
};

#endif
