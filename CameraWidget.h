#ifndef CameraWidget_H
#define CameraWidget_H

#include <QWidget>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "xiApiPlusOcv.hpp"

#include <atomic>

class CanvasWidget;
class xiAPIplusCameraOcv;

class QComboBox;
class QDir;
class QImage;
class QLabel;
class QLineEdit;
class QPushButton;
class QTimer;


class CameraWidget : public QWidget
{
    Q_OBJECT
public:
    CameraWidget( QWidget* inParent );
   ~CameraWidget();

    void InitializeCamera();

signals:

  /*
    * This signal is fired when camera is request is made to capture images.
   */
  void OnPrepareToCapture();

  /*
    * This signal is fired when reset is requested.
   */
  void OnReset();

  /*
    * This signal is fired user changes the image name.
   */
  void OnImageNamePrefixChanged( QString const & inName );

   /*
    * This signal is fired upon initialization of camera.
   */
   void OnCameraInitialized( bool );

   /*
    * This signal is fired when an image is captured.
    * The image in the signal can be used for the further processing.
   */
   void OnImageCaptured( cv::Mat const & inImage );



private:
  void SetUpUI();
  void SetUpConnections();
  int SetUpCamera();
  void StartStreaming();
private:
  // GUI controls
  QLabel* mCompanyName;
  CanvasWidget* mCanvasWidget;
  QLineEdit* mSlideName;
  QPushButton* mSetSlideName;
  QPushButton* mImageSlide;
  QPushButton* mReset;

  //Camera API object
  xiAPIplusCameraOcv mXiCam;

  // Timer
  QTimer* mCaptureTimer;
};

#endif
