#include <QCoreApplication>
#include <QSerialPort>
#include <iostream>

/*
uBloxGPS.h - Example Program demonstrating how uBloxGPS library works.
Created by Don Gibson on June 15th, 2018
SquirrelEngineering.com

Released into the public domain.
*/

#include "uBloxGPS.h"

uBloxGPS uBlox;  // The uBloxGPS object.
uint32_t LastMessage = 0; // Last Message# displayed.

QSerialPort Serial2;

void SetUpGPS();
void SetUpdate1Hz();

//
// Callback function to be called when a valid position is decoded.
//
void PositionUpdateCallback(uBloxGPS *gps)
{
    std::cout << "Callback Fix#=" << gps->FixNumber;
}

// The setup() function runs once each time the micro-controller starts
void setup()
{
    // Serial.begin(115200); // Message output
    // Serial.println("Serial Started.");
    // Serial2.begin(9600); //GPS Serial port
    // Serial.println("Serial2 Started.");

    SetUpGPS();  // Sets up the GPS messages. Turns off default NEMA messages, turns on NAV-PVT message. Does not save this to the GPS.
    SetUpdate1Hz(); // Set the update rate to 1 per second. (1Hz).

    uBlox.SetPositionUpdateCallbackFunction(&PositionUpdateCallback);  // Register a calback function to be called when a valid position fix is decoded.
}



// Add the main program code into the continuous loop() function
void loop()
{

    char ch;
    while (Serial2.isOpen())  // Data from the GPS
    {
        Serial2.read(&ch, 1); // Get next character to process
        uBlox.FeedMe(ch); // Feed the serial data into the uBloxGPS object with the FeedMe method.
    }

    if (uBlox.MessageNumber != LastMessage) // Prevent redundant displays of the same Message info.
    {
        LastMessage = uBlox.MessageNumber; // Save the Message# about to be displayed.


        // Demonstrates the more useful data fields of the class.
        std::cout << "Message#=" << uBlox.MessageNumber;

        std::cout << "Fix#=" << uBlox.FixNumber << std::endl;
        std::cout << "Is a Valid Location=" << uBlox.IsValidFixPosition() << std::endl;
        std::cout << "FixType=" << uBlox.PVT.fixType << std::endl;
        std::cout << "Valid bits=" << uBlox.PVT.valid << std::endl; // TODO format as a binary output
        std::cout << "PositionFixAge=" << uBlox.PositionFixAge() << std::endl;
        std::cout << "Time=" << uBlox.PVT.year
                  << "-" << uBlox.PVT.month
                  << "-" << uBlox.PVT.day
                  << " " << uBlox.PVT.hour
                  << ":" << uBlox.PVT.min
                  << ":" << uBlox.PVT.sec << std::endl;

        // The GPS data is typically in a integer format, so you have to scale the numbers to get the usual floating point
        // display format. See the lat & Lon calculations as an example.
        std::cout << "Pos=" << std::endl << uBlox.PVT.lat / 1e7 << "," << uBlox.PVT.lon / 1e7 << std::endl;
        std::cout << "Alt=" << uBlox.PVT.hMSL / 1e3 << std::endl; //Meters
        std::cout << "hAcc=" << uBlox.PVT.hAcc / 1e3 // Meters
                  << " "
                  << "vAcc=" << uBlox.PVT.vAcc / 1e3 << std::endl; // Meters
        std::cout << "DOP=" << uBlox.PVT.pDOP << std::endl; //
        std::cout << "Speed=" << uBlox.PVT.gSpeed / 1e3 << std::endl; // m/s
        std::cout << "Course=" << uBlox.PVT.heading / 1e5 << std::endl; //
        std::cout << "Satellites=" << uBlox.PVT.numSV << std::endl; //

    }
    else
    {
        // Nothing to do.
    }

}



// Functions to set up uBlox GPS.
// Turns off default NEMA message
// Turns on NAV-PVT Message

// These changes are not permanently saved to the GPS. The GPS will reset to defaults when power cycled.
//  Edit these functions  to use your desired Serial interface object (Hardware or Software Serial)
void SetUpGPS()
{
    // Turn off default NEMA messages
    const char  Disable_NEMAGLL[] = { 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2A };
    Serial2.write(Disable_NEMAGLL, 16);

    const char Disable_NEMAGSV[] = { 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x38 };
    Serial2.write(Disable_NEMAGSV, 16);

    const char Disable_NEMAGSA[] = { 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x31 };
    Serial2.write(Disable_NEMAGSA, 16);

    const char Disable_NEMAGGA[] = { 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x23 };
    Serial2.write(Disable_NEMAGGA, 16);

    const char Disable_NEMAVTG[] = { 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x46 };
    Serial2.write(Disable_NEMAVTG, 16);

    const char Disable_NEMARMC[16] = { 0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x3F };
    Serial2.write(Disable_NEMARMC, 16);

    // Turn on the NAV-PVT message, this is required.
    const char Enable_UBXPVT[] = { 0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x18,0xE1 };
    Serial2.write(Enable_UBXPVT, 16);
}

//
// Change the update rate to 1Hz
//
void SetUpdate1Hz()
{
    const char RateCmd[] = { 0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39 };
    Serial2.write(RateCmd, 14);
}

//
// Change the update rate to 2Hz
//
void SetUpdate2Hz()
{
    const char RateCmd[] = { 0xB5,0x62,0x06,0x08,0x06,0x00,0xF4,0x01,0x01,0x00,0x01,0x00,0x0B,0x77 };
    Serial2.write(RateCmd, 14);
}

//
// Change the update rate to 5Hz
//
void SetUpdate5Hz()
{
    const char RateCmd[] = { 0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A };
    Serial2.write(RateCmd, 14);
}

//
// Change the update rate to 10Hz
//
void SetUpdate10Hz()
{
    const char RateCmd[] = { 0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12 };
    Serial2.write(RateCmd, 14);
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.



    return a.exec();
}

