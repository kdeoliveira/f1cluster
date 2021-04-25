#include "serialization.h"

Serialization::Serialization(QObject *parent) : QObject(parent), connected(false), serial(new QSerialPort), portAvailable(false)
{
    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();


    foreach(const QSerialPortInfo &serialInfo, QSerialPortInfo::availablePorts()){
        if(serialInfo.vendorIdentifier() == arduinoUnoVendor && serialInfo.productIdentifier() == arduinoUnoProduct){
               arduinoPortName = serialInfo.portName();
               portAvailable = true;
        }
    }

    if(portAvailable){
        serial->setPortName(arduinoPortName);
        while(!serial->isOpen())
            serial->open(QSerialPort::ReadWrite);
        serial->setBaudRate(QSerialPort::Baud57600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        connected = true;
    }else{
        qDebug() << "Unable to connect to serial port";
    }
}

Serialization::~Serialization()
{
    if(serial->isOpen()) serial->close();
    qDebug() << "destructor serialization";
}

void Serialization::write(QByteArray packet)
{
    serial->waitForBytesWritten(1);

    serial->write(packet.toStdString().c_str(), packet.count());
}

void Serialization::stop()
{
    serial->flush();
    if(serial->isOpen())
        serial->close();
    connected = false;
}

bool Serialization::isConnceted() const
{
    return connected;
}

