#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include "Packets.h"

class Serialization : public QObject
{
    Q_OBJECT
public:
    explicit Serialization(QObject *parent = nullptr);
    ~Serialization();
    void write(QByteArray);
    void stop();

    bool isConnceted() const;

signals:


private:
    bool connected;
    QByteArray buffer;
    QSerialPort* serial;
    static const quint16 arduinoUnoVendor = 9025;
    static const quint16 arduinoUnoProduct = 67;
    QString arduinoPortName;
    bool portAvailable;
    PacketSerial* packet_t;
};

#endif // SERIALIZATION_H
