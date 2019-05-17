#include <SerialWorker.h>
#include "Utils.h"

SerialWorker::SerialWorker( QObject * inParent )
:
    mImage()
{
}

void
SerialWorker::SetImage( cv::Mat const & inImage )
{
    QMutexLocker lock( &mImageMutex );
    mImage = inImage;
}


// Crappy code ahead! Refactor me!
void
SerialWorker::SetupObject()
{
    auto portName = GetPortName();
    if( portName.isEmpty() )
    {
        qDebug()<<"Port for logic controller not found!";
        OnSetupDone( false );
        return;
    }
    QSerialPort serialPort( portName );
    serialPort.setBaudRate( 57600 );
    int status = serialPort.open( QIODevice::ReadWrite );
    auto error = serialPort.error();

    //A-1 : Setup variables and ask Arduino to go home:
   // GoHome
    auto writtenBytes = serialPort.write( "Q" );
    serialPort.waitForBytesWritten(50);
    if( writtenBytes != 1 )
    {
        std::cout<<"GoHome failed, terminating!"<<std::endl;
        serialPort.close();
        OnSetupDone( false );
        return;
    }
    serialPort.waitForReadyRead(5000);

    auto img = GetImage();
    if( cv::countNonZero( img ) )
        auto var = GetVariance(img);

    GoToX( "000000", serialPort );
    GoToY( "020000", serialPort );
    auto Avar=FullAF(serialPort);
    writtenBytes = serialPort.write("E" );
    serialPort.waitForBytesWritten(50);
    if( writtenBytes != 1 )
    {
        std::cout<<"Serialport Setup failed"<<std::endl;
        serialPort.close();
        OnSetupDone( false );
        return;
    }


    GoToX( "020000", serialPort );
    auto Bvar =FullAF(serialPort);
    writtenBytes = serialPort.write("R" );
    serialPort.waitForBytesWritten(50);
    if( writtenBytes != 1 )
    {
        std::cout<<"Serialport Setup failed"<<std::endl;
        serialPort.close();
        OnSetupDone( false );
        return;
    }

    GoToX( "030000", serialPort );
    auto Cvar =FullAF(serialPort);
    writtenBytes = serialPort.write("Y" );
    serialPort.waitForBytesWritten(50);
    if( writtenBytes != 1 )
    {
        std::cout<<"Serialport Setup failed"<<std::endl;
        serialPort.close();
        OnSetupDone( false );
        return;
    }

    char waitForResponse = '0';
    serialPort.waitForReadyRead(5000);
    serialPort.close();
    OnSetupDone( true );
    return;
    //fixme!
    serialPort.read( &waitForResponse, 1 );
    if( waitForResponse == 'S' )
    {
        serialPort.close();
        OnSetupDone( true );
        return;
    }
    else
    {
        serialPort.close();
        OnSetupDone( false );
        return;
    }
}

cv::Mat
SerialWorker::GetImage()
{
    QMutexLocker lock( &mImageMutex );
    auto image = mImage;
    return image;
}

double
SerialWorker::FullAF( QSerialPort & ioPort )
{
    // Step A
    auto crude_pulse_count = Z_travel_for_crude / crude_step_count;
    double crude_curr_var=0.;
    double crude_max_var=0.;
    double crude_loc_max_var=0.;

    // go to starting Z loc (we've hit limit switch via arduino already so its z=0)

    double crude_start_loc = 0 - (Z_travel_for_crude/2);
    GoToZ( QString::number(crude_start_loc).toStdString().c_str(), ioPort );

    // Step B
    //sleep for 200 ms / 0.2 sec.
    std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

    // Step C & D
    auto crude_curr_loc = crude_start_loc;
    for( int i=1; i<= crude_pulse_count; ++ i )
    {
        crude_curr_var = GetVariance( GetImage() );
        if( crude_curr_var > crude_max_var)
        {
            crude_max_var = crude_curr_var;
            crude_loc_max_var = crude_curr_loc;
        }
        crude_curr_loc = crude_curr_loc + crude_step_count;
        GoToZ( QString::number(crude_curr_loc).toStdString().c_str() ,ioPort);
    }

    GoToZ( QString::number(crude_loc_max_var).toStdString().c_str() ,ioPort);
    std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

    // Step E
    crude_pulse_count = Z_travel_for_crude / crude_step_count;
    crude_curr_var=0;
    crude_max_var=0;

    crude_start_loc = crude_loc_max_var - (Z_travel_for_crude/2);
    GoToZ( QString::number(crude_start_loc).toStdString().c_str() ,ioPort);
    std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

    crude_curr_loc = crude_start_loc;
    for( int i=1; i<= crude_pulse_count; ++ i )
    {
        crude_curr_var = GetVariance( GetImage() );
        if (crude_curr_var > crude_max_var)
        {
            crude_max_var = crude_curr_var;
            crude_loc_max_var = crude_curr_loc;
        }
        crude_curr_loc = crude_curr_loc + crude_step_count;
        GoToZ( QString::number(crude_curr_loc).toStdString().c_str() ,ioPort);
    }
    GoToZ( QString::number(crude_loc_max_var).toStdString().c_str() ,ioPort);
    std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

    //Step F
    auto currZ = crude_loc_max_var;
    auto start_above_loc = currZ - (Ztotaltravel/2);
    GoToZ( QString::number(start_above_loc).toStdString().c_str() ,ioPort);
    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    // A vector( array like container ) with Ztotaltravel number of zeros filled.
    std::vector< double > localAF_loc_array( 0, Ztotaltravel );
    std::vector< double > localAF_var_array( 0, Ztotaltravel );
    auto localAF_curr_loc = start_above_loc;

    for( int i=0; i<Ztotaltravel/5; ++i )
    {
        localAF_loc_array[i] = localAF_curr_loc;
        localAF_var_array[i]= GetVariance( GetImage() );
        localAF_curr_loc = localAF_curr_loc + 5;
        GoToZ( QString::number(localAF_curr_loc).toStdString().c_str() ,ioPort);
    }
    auto result = std::max_element( localAF_var_array.begin(), localAF_var_array.end() );

    return *result;
}
