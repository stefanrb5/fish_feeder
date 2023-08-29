#include "dialog.h"
#include "ui_dialog.h"
#include <wiringPi.h>
#include <unistd.h>
#include <fcntl.h>
#include <QTimer>
#include <cstdlib>

// Define the GPIO pin for the servo
#define SERVO_PIN 1   // WiringPi pin 1 (BCM pin 18)

//Define the GPIO for the SRC04
#define TRIG_PIN 0
#define ECHO_PIN 2

float distance;
int integer, decimal;

Dialog::Dialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->pushButton->setEnabled(true);

    timer=new QTimer(this);

    connect (timer, SIGNAL(timeout()), this,SLOT(printNumber()));
    connect (timer, SIGNAL(timeout()), this,SLOT(printDistance()));
    connect(timer, SIGNAL(timeout()), this, SLOT(on_progressBar_valueChanged())); // Connect to the slot without the value parameter
    timer->setInterval(1000);
    timer->start();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    pinMode(SERVO_PIN, PWM_OUTPUT);

    // Set the PWM frequency (50 Hz) for the servo
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(200);    // 20 ms (50 Hz) / 100 = 10 ms per step
    pwmSetClock(1920);   // 19.2 MHz / 1920 = 10 kHz

    ui->pushButton->setEnabled(false);

    pwmWrite(SERVO_PIN, 25); // Set the servo to 0 degrees

    QTimer::singleShot(1000, this, [this]() {
        pwmWrite(SERVO_PIN, 125); // Set the servo to 180 degrees
        ui->pushButton->setEnabled(true);
    });
}

long readTemperature()
{
    int fd = -1, ret;
    char *tmp1, tmp2[10], ch = 't';
    char devname_head[50] = "/sys/devices/w1_bus_master1/28-3cb0f649967f";

    char devname_end[10] = "/w1_slave";
    char dev_name[100];
    long value;
    char buffer[100];

    strcpy(dev_name, devname_head);
    strcat(dev_name, devname_end);

    if ((fd = open(dev_name, O_RDONLY)) < 0)
    {
        perror("Error while opening!");
        exit(1);
    }

    ret = read(fd, buffer, sizeof(buffer));

    if (ret < 0)
    {
        perror("Greška pri čitanju!");
        exit(1);
    }

    tmp1 = strchr(buffer, ch);
    sscanf(tmp1, "t=%s", tmp2);
    value = atoi(tmp2);

    close(fd);

    return value;
}

void Dialog::printNumber()
{
    integer = readTemperature() / 1000;
    decimal = readTemperature() % 1000;
    number="%1.%2";

    ui->lcdNumber->display(number.arg(integer).arg(decimal));
}

float Dialog::getDistance()
{
    // Trigger the ultrasonic sensor by sending a 10us pulse on the TRIG pin
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Wait for the ECHO pin to go high
    while (digitalRead(ECHO_PIN) == LOW);

    // Measure the duration the ECHO pin is high
    long startTime = micros();
    while (digitalRead(ECHO_PIN) == HIGH);
    long travelTime = micros() - startTime;

    // Calculate the distance based on the speed of sound (343 meters per second)
    // and the time it took for the sound to travel back and forth
    float distance = (float)travelTime * 0.0343 / 2;

    return distance;
}

void Dialog::printDistance()
{
    distance = getDistance()-6.87; //6.87 is the height of the aquarium
    float waterLevel = abs(distance); // Call the getDistance function to get the distance value.

    // Convert the distance to a QString to display it on lcdNumber_2
    QString distanceStr = QString::number(waterLevel, 'f', 2); // Show two decimal places.

    // Set the QString value to lcdNumber_2 for display.
    ui->lcdNumber_2->display(distanceStr);
}

void Dialog::on_progressBar_valueChanged()
{
    float waterLevel = abs(distance);
    // Calculate the percentage based on the distance (assuming 6cm = 100% and 0cm = 0%)
    int percentage = static_cast<int>((waterLevel / 6.0) * 100);

    // Ensure the percentage stays within the valid range (0% to 100%)
    percentage = std::max(0, std::min(100, percentage));

    // Update the progressBar with the calculated percentage
    ui->progressBar->setValue(percentage);
}
