#include "mainwindow.h"
#include "ui_mainwindow.h"

// for cout and endl
#include <cstdlib>
#include <iostream>

#include <QDebug>

// Wiring Pi library
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>

// Project includes
#include "GpsBoatC/includes.h"
#include "GpsBoatC/config.h"
#include "GpsBoatC/TinyGPS.h"
#include "GpsBoatC/Arduino.h"

//-----------------------------------------------------------------------------
// local function prototypes

// Threads
PI_THREAD 	(THREAD_UpdateGps);

//-----------------------------------------------------------------------------
// local data
typedef struct
{
    float flat;
    float flon;
    float fmph;
    float fcourse;
    U8 hour;
    U8 minute;
    U8 second;
    bool bGpsLocked;
} tGPS_INFO;

tGPS_INFO gtGpsInfo;

#define GPS_DATA_KEY	0

int gSerial_fd;

// Arduino on I2C bus
Arduino cArduino;

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup
    // connect GPS update timer slot
    connect(&timer_GpsUpdate, SIGNAL(timeout()), this, SLOT(GpsUpdate()));
    this->timer_GpsUpdate.start(1000);

    //-----------------------
    printf("GPS ...\n");

    if ((gSerial_fd = serialOpen ("/dev/ttyAMA0", GPS_BAUD)) < 0)
    {
       std::cerr << "Unable to open serial device\n";
       return;
    }
    else
    {
       std::cout << "Comm port to GPS opened. GPS Baud: " << GPS_BAUD << std::endl;

       // Start the GPS thread
       piThreadCreate( THREAD_UpdateGps );
    }

    //-----------------------
    printf("Aurdino ...\n");

    cArduino.Init( ARDUINO_I2C_ADDR );

    printf("\tArduino version: 0x%X\n", cArduino.GetReg( ARDUINO_REG_VERSION ) );

    printf("OK\n");
}

//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    this->timer_GpsUpdate.stop();

    delete ui;
}

//-----------------------------------------------------------------------------
void MainWindow::on_pushButton_Start_clicked()
{
    if(ui->pushButton_Start->text() == QString("Start"))
    {
        this->timer_GpsUpdate.start(1000);
        ui->pushButton_Start->setText(QString("Stop"));
    }
    else
    {
        this->timer_GpsUpdate.stop();
        ui->pushButton_Start->setText(QString("Start"));
    }

}

//-----------------------------------------------------------------------------
void MainWindow::on_pushButton_ServoLeft_clicked()
{
    cArduino.SetReg( ARDUINO_REG_STEERING, 0);
}

//-----------------------------------------------------------------------------
void MainWindow::on_pushButton_ServoRight_clicked()
{
    cArduino.SetReg( ARDUINO_REG_STEERING, 255);
}

//-----------------------------------------------------------------------------
void MainWindow::on_pushButton_Ard_ToggleLed_clicked()
{
    static int toggle = 0;

    toggle += 1;

    if( (toggle & 0x1) )
    {
        cArduino.SetReg( ARDUINO_REG_EXTRA_LED, 1 );
    }
    else
    {
        cArduino.SetReg( ARDUINO_REG_EXTRA_LED, 0 );
    }
}

//-----------------------------------------------------------------------------
void MainWindow::GpsUpdate()
{
    static float flat=0, flong=0;

    if( gtGpsInfo.bGpsLocked )
    {
        ui->label_GpsStatus->setText(QString("Locked"));
    }
    else
    {
        ui->label_GpsStatus->setText(QString("Not Locked"));
    }

    flat = gtGpsInfo.flat;
    flong = gtGpsInfo.flon;

    ui->lineEdit_Lat->setText(QString::number(flat, 'f', 6));

    ui->lineEdit_Long->setText(QString::number(flong, 'f', 6));
}

//-----------------------------------------------------------------------------------
// Returns valid cGps data if GPS has a Fix
PI_THREAD (THREAD_UpdateGps )
{
    //static bool bLocked = false;
    bool bNewGpsData = false;
    unsigned long fix_age;
    //int year;
    //U8 month, day, hundredths;

    // GPS
    TinyGPS cGps;

    std::cout << "THREAD_UpdateGps started\n";

    while( true )
    {
        delay( 200 );

        // *******************************
        // Grab GPS data from serial input
        // *******************************
        while( serialDataAvail(gSerial_fd) )
        {
            char c;
            c = serialGetchar(gSerial_fd);

#if DO_GPS_TEST
            printf("%c", c);
            fflush( stdout );
#endif
            if (cGps.encode(c))
            {
                bNewGpsData = true;
            }
        }

        // ********************
        // Process new cGps info
        // ********************
        if( bNewGpsData )
        {
            // Lock the GPS data for updating
            piLock( GPS_DATA_KEY );

            // GPS Position
            // retrieves +/- lat/long in 100000ths of a degree
            cGps.f_get_position( &gtGpsInfo.flat, &gtGpsInfo.flon, &fix_age);

            if (fix_age == TinyGPS::GPS_INVALID_AGE)
            {
                gtGpsInfo.bGpsLocked = false;
            }
            else
            {
                gtGpsInfo.bGpsLocked = true;
            }

#if USE_GPS_TIME_INFO
            // GPS Time
            cGps.crack_datetime(&year, &month, &day, &gtGpsInfo.hour, &gtGpsInfo.minute, &gtGpsInfo.second, &hundredths, &fix_age);
#endif // USE_GPS_TIME_INFO

            // GPS Speed
            gtGpsInfo.fmph = cGps.f_speed_mph(); // speed in miles/hr
            // course in 100ths of a degree
            gtGpsInfo.fcourse = cGps.f_course();

            // Unlock the GPS data for updating
            piUnlock( GPS_DATA_KEY );

            // reset new data flag
            bNewGpsData = false;
        }
    }
    return 0;
}





