#ifndef UTILS_H
#define UTILS_H

#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QImage>
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStatusBar>

#include <chrono>
#include <thread>

#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>



char const* kHandShakeChar = "7";
char const* kGoToX = "B";
char const* kGoToY = "N";
char const* kGoToZ = "M";

const std::size_t Zmax=30000;
const std::size_t Xmax=300000;
const std::size_t Ymax=300000;

const std::size_t Z_travel_for_crude = 5000;	// USE A RIGHT VALUE HERE
const std::size_t crude_step_count = 1000;

const std::size_t Ztotaltravel = 200;

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

bool DoHandShake( QSerialPort & ioPort )
{
    ioPort.flush();
    ioPort.clear();
    auto status = ioPort.write( kHandShakeChar );
    ioPort.waitForBytesWritten(50);
    if( status != -1 )
    {
        ioPort.waitForReadyRead(5000);
        auto data = ioPort.readAll();
        return true;
        return data[0] == '7';
        //return handhshakeResponse[0] == '7';
    }
}

bool GoToX( QByteArray const & inArray, QSerialPort & ioPort )
{
    if( DoHandShake( ioPort ) )
    {
        auto writtenBytesHandshake = ioPort.write( kGoToX );
        ioPort.waitForBytesWritten(50);

        auto writtenBytesData = ioPort.write( inArray );
        ioPort.waitForBytesWritten(50);
        return
                writtenBytesHandshake != -1
                &&
                writtenBytesData != -1;
    }
    qDebug()<<"Handhshake failed while GoToX";
    return false;
}

bool GoToY( QByteArray const & inArray, QSerialPort & ioPort )
{
    if( DoHandShake( ioPort ) )
    {
        auto writtenBytesHandshake = ioPort.write( kGoToY );
        ioPort.waitForBytesWritten(50);
        auto writtenBytesData = ioPort.write( inArray );
        ioPort.waitForBytesWritten(50);
        return
                writtenBytesHandshake != -1
                &&
                writtenBytesData != -1;
    }
    qDebug()<<"Handhshake failed while GoToY";
    return false;
}

bool GoToZ( QByteArray const & inArray, QSerialPort & ioPort )
{
    if( DoHandShake( ioPort ) )
    {
        auto writtenBytesHandshake = ioPort.write( kGoToZ );
        ioPort.waitForBytesWritten(50);
        auto writtenBytesData = ioPort.write( inArray );
        ioPort.waitForBytesWritten(50);
        return
                writtenBytesHandshake != -1
                &&
                writtenBytesData != -1;
    }
    qDebug()<<"Handhshake failed while GoToZ";
    return false;
}

double GetVariance( cv::Mat const & inImg )
{
    cv::Mat test = inImg;

    cv::cvtColor(test, test, CV_BGR2HSV);

    int v_bins = 50;
    int histSize[] = { v_bins };
    cv::MatND hist;

    float v_ranges[] = { 0, 255 };
    cv::vector<cv::Mat> channel(3);
    split(test, channel);

    const float* ranges[] = { v_ranges };
    int channels[] = { 0 };

    cv::calcHist(&channel[2], 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false); //histogram calculation

    cv::Scalar mean, stddev;
    cv::meanStdDev(hist, mean, stddev);

    std::cout << "Mean: " << mean[0] << "   StdDev: " << stddev[0] << std::endl;
    return stddev[0]*stddev[0];
}

#endif // UTILS_H
