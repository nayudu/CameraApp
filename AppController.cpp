#include "AppController.h"
#include "CameraWidget.h"
#include "AppUI.h"
#include "SerialWorker.h"

#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QImage>
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStatusBar>
#include <QThread>

#include <chrono>
#include <thread>

#include <QDebug>

namespace
{
    QString const kInitializingFailed = "Camera Initialization Failed.";
    QString const kInitializingSuccessful = "Camera Successfully Initialized.";

    const std::size_t XFoVSize = 350;
    const std::size_t YFoVSize = 450;

    //MICRO METER PER SEC
    const std::size_t XPulseRate = 3800;
    const std::size_t YPulseRate = 3800;

    const double Xzadjustdelay = 0.002;
    const double autoContrTime = 0.1;
    const int XImDelay = XFoVSize / XPulseRate;
    const int YImDelay = YFoVSize / YPulseRate;

    const double numofX = 50000 / XFoVSize;
    const double numofY = 25000 / YFoVSize;

    QString GetPortName()
    {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            if( info.manufacturer().contains("arduino",Qt::CaseInsensitive ) )
            {
                return info.portName();
            }
        }
        return QString();
    }
}

AppController::AppController( AppUI& inMainWindow )
:
  mMainWindow( inMainWindow ),
  mCameraWidget( new CameraWidget( &inMainWindow ) ),
  mEnableSave( false ),
  mDir( nullptr ),
  mImageNamePrefix(),
  mCounter(0),
  mYCounter(0),
  mImg(),
  mImagesToSave()

{
  inMainWindow.setCentralWidget( mCameraWidget );
  connect
  (
    mCameraWidget,
    &CameraWidget::OnCameraInitialized,
    [ this ]( bool inSuccess )
    {
      mMainWindow.statusBar()->showMessage
      (
          inSuccess
          ?
              kInitializingSuccessful
              :
              kInitializingFailed
      );
    }
  );

  connect
  (
    mCameraWidget,
    &CameraWidget::OnImageCaptured,
    [ this ]( cv::Mat const & inImage )
    {
      ProcessImage( inImage );
    }
  );

  connect
  (
    mCameraWidget,
    &CameraWidget::OnReset,
    [ this ]()
    {
      auto portName = GetPortName();
      if( portName.isEmpty() )
      {
          mMainWindow.statusBar()->showMessage( "No Arduino port available..." );
          return;
      }
      QSerialPort serialPort( portName );
      serialPort.setBaudRate( 57600 );
      int status = serialPort.open( QIODevice::ReadWrite );
      if( status )
      {
          mMainWindow.statusBar()->showMessage( "Reset successful..." );
          serialPort.close();
      }
      else
      {
        mMainWindow.statusBar()->showMessage( "Port not accessible. Reset failed..." );
      }
    }
  );

  connect
  (
    mCameraWidget,
    &CameraWidget::OnPrepareToCapture,
    [ this ]
    {
      auto shouldSave = !mEnableSave;
      if( shouldSave )
      {
          mMainWindow.statusBar()->showMessage( "Setting up the object..." );
          QThread* thread = new QThread;
          SerialWorker* worker = new SerialWorker();
          worker->SetImage( mImg );
          worker->moveToThread(thread);

          connect
          (
            thread,
            &QThread::started,
            worker,
            &SerialWorker::SetupObject
          );

          connect
          (
            mCameraWidget,
            &CameraWidget::OnImageCaptured,
            worker,
            &SerialWorker::SetImage
          );

          connect
          (
            worker,
            &SerialWorker::OnSetupDone,
            [ this, shouldSave ]( bool inResult )
            {
              if( inResult )
              {
                  delete mDir;
                  mDir = nullptr;
                  mCounter = 0;

                  mEnableSave = shouldSave;
                  mMainWindow.statusBar()->showMessage( "Saving Images to disk..." );
              }
              else
              {
                  mMainWindow.statusBar()->showMessage( "Object SetUp failed..." );
              }
            }
          );

          connect
          (
            thread,
            &QThread::started,
            worker,
            &SerialWorker::SetupObject
          );

          connect
          (
            worker,
            &SerialWorker::OnSetupDone,
            thread,
            &QThread::quit
          );

          connect
          (
            worker,
            &SerialWorker::OnSetupDone,
            worker,
            &SerialWorker::deleteLater
          );


          connect
          (
            thread,
            &QThread::started,
            worker,
            &SerialWorker::SetupObject
          );

          connect
          (
            thread,
            &QThread::finished,
            thread,
            &QThread::deleteLater
          );
          thread->start();
      }
    }
  );

  connect
  (
    mCameraWidget,
    &CameraWidget::OnImageNamePrefixChanged,
    [ this ]( QString const & inName )
    {
      mImageNamePrefix = inName;
    }
  );

  mCameraWidget->InitializeCamera();
}

void
AppController::ProcessImage( cv::Mat const & inImage )
{
    mImg = inImage;
    if( mEnableSave )
    {
      QString timeStamp = QDateTime::currentDateTime().toString();
      if( !mDir )
      {
          if( !mImageNamePrefix.isEmpty() )
          {
            mDir = new QDir( QDir::home().canonicalPath() + "/" + timeStamp + "/" );
            mDir->mkdir( QDir::home().canonicalPath() + "/" + timeStamp + "/" );
            mDir->mkdir( QDir::home().canonicalPath() + "/" + timeStamp + "/" + "tiles" );
            mDir->cd( QDir::home().canonicalPath() + "/" + timeStamp + "/" + "tiles" + "/" );
          }
          else
          {
              mMainWindow.statusBar()->showMessage( "Save failed, Please set a slide name..." );
              return;
          }
      }
      // Use .png for saving, it is lossless format!
      QString fileName = mDir->path() + "/" + mImageNamePrefix + "_" + QString::number( mYCounter ) + "_" + QString::number( mCounter ) +  ".png";
      mCounter++;
      // Delay post every image capture
      std::this_thread::sleep_for( std::chrono::milliseconds( XImDelay ) );

      if( mCounter == numofX )
      {
          mCounter = 0;
          mYCounter++;
          // Delay post every strip capture
          std::this_thread::sleep_for( std::chrono::milliseconds( YImDelay ) );
      }
      imwrite( fileName.toStdString().c_str(), inImage );
      mMainWindow.statusBar()->showMessage( "Saving File: " + fileName );
    }
}

