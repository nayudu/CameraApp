#ifndef CanvasWidget_H
#define CanvasWidget_H

#include <QWidget>
#include <QSize>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

class QImage;
class QLabel;
class QPaintEvent;

class CanvasWidget : public QWidget
{
public:
    CanvasWidget( QWidget* inParent );
    void SetImage( cv::Mat const & inImage );
    ~CanvasWidget();
private:
    QLabel* mImageLabel;
};

#endif // CANVASWIDGET_H
