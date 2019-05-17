#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QMutex>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

class QSerialPort;

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    SerialWorker( QObject * inParent=nullptr );
    void SetImage( cv::Mat const & inImage );
    void SetupObject();
signals:
    void OnSetupDone( bool );
private:
    cv::Mat GetImage();
    double FullAF( QSerialPort & ioPort );
private:
    QMutex mImageMutex;
    cv::Mat mImage;
};

#endif // SERIALWORKER_H
