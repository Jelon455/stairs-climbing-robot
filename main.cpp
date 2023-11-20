#include <hFramework.h>
#include <DistanceSensor.h>
#include <Lego_Touch.h>
#include <sys/time.h>

using namespace hSensors;
using namespace hModules;

// hMot1 is the motor for the front wheels
// hMot2 is the motor for the rear wheels
// hMot3 is the motor for the crawler track
hLegoSensor_simple touch(hSens5);
Lego_Touch sensor(touch);          // sensor for setting mode
DistanceSensor frontLSens(hSens1); // front sensor
int distL = 0;

void errorDistanceSensor()
{
    distL = frontLSens.getDistance();

    while (distL == -1)
    {
        distL = frontLSens.getDistance();
        sys.delay(100);
    }
}

void findStairs()
{
    sys.delay(200);

    distL = frontLSens.getDistance();
    errorDistanceSensor();
    // approach to the step
    while (distL > 3)
    {
        distL = frontLSens.getDistance();
        errorDistanceSensor();

        sys.delay(50);

        hMot1.setPower(700);
        hMot2.setPower(700);
    }
    // align wheels at the step
    sys.delay(1500);
}

void placeForCrawlerTrack()
{
    // robot on the stairs and moving back to prevent tilting the crawler track
    while (distL < 6)
    {
        hMot1.setPower(-150);
        hMot2.setPower(50);
        hMot3.setPower(0);

        distL = frontLSens.getDistance();
        errorDistanceSensor();

        sys.delay(50);
    }
    // robot on the stairs and leveling if the sensor incorrectly caught the value to prevent tilting the crawler track
    while (distL >= 6)
    {
        hMot1.setPower(150);
        hMot2.setPower(50);
        hMot3.setPower(0);

        distL = frontLSens.getDistance();
        errorDistanceSensor();

        sys.delay(20);
    }
}

void twoWheelsOnTop()
{
    // second stage of the first stage - climbing, engaging the crawler track
    distL = frontLSens.getDistance();

    while (distL > 8)
    {
        hMot1.setPower(1000);
        hMot2.setPower(100);
        hMot3.setPower(400);

        distL = frontLSens.getDistance();
        errorDistanceSensor();

        sys.delay(50);
    }
}

void goUp()
{
    findStairs();

    // first stage front wheel full - acceleration and slight tilt back
    hMot1.setPower(1000);
    hMot2.setPower(1000);
    hMot3.setPower(-100);
    sys.delay(4300);

    twoWheelsOnTop();

    placeForCrawlerTrack();

    // crawler track up
    hMot1.setPower(60);
    hMot2.setPower(0);
    hMot3.setPower(-380);
    sys.delay(7800);
    hMot3.setPower(0);
}

void hMain()
{
    int howManyStairs = 0;

    // Pressing the touch sensor starts the system
    while (hBtn1.isPressed() == false)
    {
        sys.delay(100);
    }
    Serial.printf("hBtn1 pressed START SYSTEM");
    sys.delay(600);

    while (hBtn2.isPressed() == false)
    {
        // Check if hBtn2 is pressed
        if (sensor.isPressed())
        {
            howManyStairs++;
            Serial.printf("Number of stairs %d", howManyStairs);
            sys.delay(500);
        }
        else
        {
            // do nothing
        }
    }
    // autonomous mode
    for (;;)
    {
        goUp();
    }
    // specified number of steps
    else
    {
        for (int i = 1; i <= (howManyStairs); i++)
        {
            goUp();
        }
        // flashing LEDs to indicate the end of stair climbing
        while (hBtn1.isPressed() == false)
        {
            hLED1.toggle();
            hLED2.toggle();
            hLED3.toggle();
            sys.delay(200);
        }
    }
}
