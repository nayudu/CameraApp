# include "CanvasWidget.h"

#include <QImage>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>

#include <QDebug>
#include <QPixmap>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

namespace
{
    std::size_t const kWidth = 1920;
    std::size_t const kHeight = 1080;

    QImage Mat2QImage(cv::Mat const& src)
    {
         cv::Mat temp; // make the same cv::Mat
         cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
         QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
         dest.bits(); // enforce deep copy, see documentation
         // of QImage::QImage ( const uchar * data, int width, int height, Format format )
         return dest;
    }
}

CanvasWidget::CanvasWidget( QWidget* inParent )
:
    QWidget( inParent ),
    mImageLabel( new QLabel( this ) )
{

   // mImageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mImageLabel->setAlignment( Qt::AlignHCenter );

    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->addWidget( mImageLabel, Qt::AlignHCenter );

    setMaximumSize( QSize( 800, 600 ) );
    setMinimumSize( QSize( 640, 480 ) );
}
void
CanvasWidget::SetImage( cv::Mat const & inImage )
{
    QImage image = Mat2QImage( inImage );
    mImageLabel->setPixmap( QPixmap::fromImage( image ).scaled( this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
}

CanvasWidget::~CanvasWidget()
{
}
