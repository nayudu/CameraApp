#include "CameraWidget.h"
#include "CanvasWidget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMediaMetaData>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "xiApiPlusOcv.hpp"

// Ximea Cam API
//////////////////////////////
#include "stdafx.h"

#ifdef WIN32
#include "xiApi.h"       // Windows
#else
#include <m3api/xiApi.h> // Linux, OSX
#endif

#include <memory>
#include <thread>
//////////////////////////////

using namespace cv;

namespace
{
  QString const kCameraCaptureText = "Capture";
  QString const kCameraInitializedText = "Camera Initialized.";
  QString const kCameraDetectioFailedText = "No Camera Detected.";
}

CameraWidget::CameraWidget( QWidget* inParent )
:
  QWidget( inParent ),
  mCompanyName( new QLabel( "IAD - Shanmukha Innovations" ) ),
  mCanvasWidget( new CanvasWidget( this ) ),
  mSlideName( new QLineEdit( this ) ),
  mSetSlideName( new QPushButton( "Set Slide Name", this ) ),
  mImageSlide( new QPushButton( "Image Slide", this ) ),
  mReset( new QPushButton( "Reset", this ) ),
  mXiCam(),
  mCaptureTimer( new QTimer( this ) )
{
  SetUpUI();
  SetUpConnections();
}

CameraWidget::~CameraWidget()
{
     mXiCam.StopAcquisition();
    mXiCam.Close();
  //delete mXiCam;
}

void
CameraWidget::InitializeCamera()
{
    if( SetUpCamera() == 0 )
    {
      StartStreaming();
    }
}

void
CameraWidget::SetUpUI()
{
    QVBoxLayout * layout = new QVBoxLayout();
    setLayout( layout );

    QWidget* companyNameWidget = new QWidget();
    companyNameWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    companyNameWidget->setMinimumHeight( 50 );
    QVBoxLayout * companyNameWidgetLayout = new QVBoxLayout();
    companyNameWidgetLayout->addWidget( mCompanyName, Qt::AlignHCenter );
    mCompanyName->setAlignment( Qt::AlignHCenter );
    companyNameWidget->setLayout( companyNameWidgetLayout );

    layout->addWidget( companyNameWidget );


    QWidget* content = new QWidget();
    QHBoxLayout * contentLayout = new QHBoxLayout();
    content->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    content->setLayout( contentLayout );
    contentLayout->addWidget( mCanvasWidget, Qt::AlignRight );

    QWidget* sideBar = new QWidget();
    sideBar->setMinimumWidth( 200 );
    QVBoxLayout * sideBarLayout = new QVBoxLayout();
    sideBar->setLayout( sideBarLayout );
    sideBar->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    sideBarLayout->addWidget( mSlideName, Qt::AlignTop );
    sideBarLayout->addWidget( mSetSlideName, Qt::AlignTop );
    sideBarLayout->addWidget( mImageSlide, Qt::AlignTop );
    sideBarLayout->addWidget( mReset, Qt::AlignTop );

    contentLayout->addWidget( sideBar, Qt::AlignRight );

    layout->addWidget( content );
//    QVBoxLayout* mainLayout = new QVBoxLayout();
//    mCompanyName->setAlignment( Qt::AlignCenter );
//    mCompanyName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
//    mainLayout->setSizeConstraint(QLayout::SetMaximumSize);
//    mainLayout->addWidget( mCompanyName, Qt::AlignCenter );
//    mainLayout->addWidget( mCanvasWidget, Qt::AlignHCenter );
//    mainLayout->addWidget( mSlideName, Qt::AlignCenter );
//    mSlideName->setPlaceholderText( "Set Slide Name..." );
//    mainLayout->addWidget( mSetSlideName, Qt::AlignCenter );
//    mainLayout->addWidget( mImageSlide, Qt::AlignCenter );
//    setLayout( mainLayout );
}

void
CameraWidget::SetUpConnections()
{
    connect
    (
        mImageSlide,
        &QPushButton::clicked,
        [ this ]
        {
            OnPrepareToCapture();
        }
    );
    connect
    (
        mSetSlideName,
        &QPushButton::clicked,
        [ this ]
        {
            OnImageNamePrefixChanged( mSlideName->text() );
        }
    );
    connect
    (
        mReset,
        &QPushButton::clicked,
        [ this ]
        {
            OnReset();
        }
    );
}

int
CameraWidget::SetUpCamera()
{
  try
  {
      // Sample for XIMEA OpenCV
      // mXiCam = new xiAPIplusCameraOcv();

      // Retrieving a handle to the camera device
      printf("Opening first camera...\n");
      mXiCam.OpenFirst();
      // Set exposure
      mXiCam.SetExposureTime(360); // 10000 us = 10 ms
      mXiCam.EnableWhiteBalanceAuto();
      mXiCam.StartAcquisition();
      mXiCam.SetImageDataFormat( XI_RGB32 );
  }
  catch(xiAPIplus_Exception& exp)
  {
      printf("Error:\n");
      exp.PrintError();
#ifdef WIN32
      Sleep(2000);
#endif
      cvWaitKey(2000);
      OnCameraInitialized( false );
      return -1;
  }
  OnCameraInitialized( true );
  return 0;
}

void
CameraWidget::StartStreaming()
{
  mCaptureTimer->setInterval( 50 );
  mCaptureTimer->start();
  connect
  (
      mCaptureTimer,
      &QTimer::timeout,
      [ this ]
      {

          XI_IMG_FORMAT format = mXiCam.GetImageDataFormat();
          cv::Mat capturedImage = mXiCam.GetNextImageOcvMat();
          if ( format == XI_RAW16 || format == XI_MONO16 )
              normalize( capturedImage, capturedImage, 0, 65536, NORM_MINMAX, -1, Mat()); // 0 - 65536, 16 bit unsigned integer range
          mCanvasWidget->SetImage( capturedImage );
          OnImageCaptured( capturedImage );


      }
  );
}
