#ifndef PACKETS_H
#define PACKETS_H

#include <QObject>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <QDataStream>
#include <map>
#include <QDebug>

enum PacketSize{
    header = 23,
    motion = 1320,
    session = 126,
    lapData = 820,
    event = 9,
    participants = 1081,
    carSetup = 820,
    carTelemetry = 1324,
    carStatus = 1120
};


typedef std::map<int, int> packetMap;
static packetMap _packetMap = {
    {-1, PacketSize::header},
    {0, PacketSize::motion},
    {1, PacketSize::session},
    {2, PacketSize::lapData},
    {3, PacketSize::event},
    {4, PacketSize::participants},
    {5, PacketSize::carSetup},
    {6, PacketSize::carTelemetry},
    {7, PacketSize::carStatus},
};



struct PacketHeader{
    quint16    m_packetFormat;         // 2019
    quint8     m_gameMajorVersion;     // Game major version - "X.00"
    quint8     m_gameMinorVersion;     // Game minor version - "1.XX"
    quint8     m_packetVersion;        // Version of this packet type, all start from 1
    quint8     m_packetId;             // Identifier for the packet type, see below
    quint64    m_sessionUID;           // Unique identifier for the session
    float       m_sessionTime;          // Session timestamp
    uint        m_frameIdentifier;      // Identifier for the frame the data was retrieved on
    quint8     m_playerCarIndex;       // Index of player's car in the array

    friend QDataStream &operator>>(QDataStream &data, PacketHeader* packet){

        data.readRawData((char *)&packet->m_packetFormat, sizeof(quint16));
        data.readRawData((char *)&packet->m_gameMajorVersion, sizeof(quint8));
        data.readRawData((char *)&packet->m_gameMinorVersion, sizeof(quint8));
        data.readRawData((char *)&packet->m_packetVersion, sizeof(quint8));
        data.readRawData((char *)&packet->m_packetId, sizeof(quint8));
        data.readRawData((char *)&packet->m_sessionUID, sizeof(quint64));
        data.readRawData((char *)&packet->m_sessionTime, sizeof(float));
        data.readRawData((char *)&packet->m_frameIdentifier, sizeof(uint));
        data.readRawData((char *)&packet->m_playerCarIndex, sizeof(qint8));

        return data;
    }
};

struct PacketSerial{
    int id = 0;
    quint8 rtr = 0;
    quint8 ide = 0;
    size_t dlc = 8;
    quint8 dataArray[8] ={0,0,0,0,0,0,0,0};

    PacketSerial &operator=(const QByteArray &src){
        if(src.count() < 20)    return *this;
        //Format expected = XXXXYYYYYYYYYYYYYYYY
        this->id = src.left(4).toUInt(nullptr, 16);
        for(size_t x=0 ; x < this->dlc; x++)
            this->dataArray[x] = src.mid(x+4, 2).toUInt(nullptr,16);
        return *this;
    }

    friend QByteArray &operator>>(QByteArray& src, PacketSerial *dest){
        if(src.count() < 19)    return src;
        //Format expected = XXXXYYYYYYYYYYYYYYYY
        dest->id = src.left(3).toUInt(nullptr, 16);

        for(size_t x=0,i=0 ; i < dest->dlc; x+=2,i++){
            dest->dataArray[i] = src.mid(x+3, 2).toUShort(nullptr, 16);
        }
        return src;
    }

    QByteArray toBytes(){
        QString str = "0";
        str.append(QString::number(id, 16));
        str.append(",");
        str.append("0"+QString::number(rtr, 16));
        str.append(",");
        str.append("0"+QString::number(ide, 16));
        str.append(",");
        for(size_t x = 0; x < dlc ; x++){
            if(dataArray[x] < 16)
                str.append('0');
            str += QString::number(dataArray[x],16);
        }
        str.append('\n');

        return str.toLatin1();
    }
};

struct PacketInterface{
    virtual int packetId() const= 0;
};

struct CarTelemetryData
{
    quint16    m_speed;                    // Speed of car in kilometres per hour
    float     m_throttle;                 // Amount of throttle applied (0.0 to 1.0)
    float     m_steer;                    // Steering (-1.0 (full lock left) to 1.0 (full lock right))
    float     m_brake;                    // Amount of brake applied (0.0 to 1.0)
    quint8     m_clutch;                   // Amount of clutch applied (0 to 100)
    qint8      m_gear;                     // Gear selected (1-8, N=0, R=-1)
    quint16    m_engineRPM;                // Engine RPM
    quint8     m_drs;                      // 0 = off, 1 = on
    quint8     m_revLightsPercent;         // Rev lights indicator (percentage)
    quint16    m_brakesTemperature[4];     // Brakes temperature (celsius)
    quint16    m_tyresSurfaceTemperature[4]; // Tyres surface temperature (celsius)
    quint16    m_tyresInnerTemperature[4]; // Tyres inner temperature (celsius)
    quint16    m_engineTemperature;        // Engine temperature (celsius)
    float     m_tyresPressure[4];         // Tyres pressure (PSI)
    quint8     m_surfaceType[4];           // Driving surface, see appendices

    friend QDataStream &operator>>(QDataStream &data, CarTelemetryData *packet){
         data.readRawData((char *)&packet->m_speed, sizeof(quint16));
        data.readRawData((char *)&packet->m_throttle, sizeof(float));
        data.readRawData((char *)&packet->m_steer, sizeof(float));
        data.readRawData((char *)&packet->m_brake, sizeof(float));
        data.readRawData((char *)&packet->m_clutch, sizeof(quint8));

        data.readRawData((char *)&packet->m_gear, sizeof(quint8));
        data.readRawData((char *)&packet->m_engineRPM, sizeof(quint16));
        data.readRawData((char *)&packet->m_drs, sizeof(quint8));
        data.readRawData((char *)&packet->m_revLightsPercent, sizeof(quint8));
        data.readRawData((char *)packet->m_brakesTemperature, sizeof(quint16)*4);
        data.readRawData((char *)packet->m_tyresSurfaceTemperature, sizeof(quint16)*4);
        data.readRawData((char *)packet->m_tyresInnerTemperature, sizeof(quint16)*4);
        data.readRawData((char *)&packet->m_engineTemperature, sizeof(quint16)*4);
        data.readRawData((char *)packet->m_tyresPressure, sizeof(float)*4);
        data.readRawData((char *)packet->m_surfaceType, sizeof(quint8)*4);


        return data;
    }
};

struct CarTelemetryDataPacket : public PacketInterface
{
    CarTelemetryData	m_carTelemetryData[20];

    quint32              m_buttonStatus;        // Bit flags specifying which buttons are being pressed
                                               // currently - see appendices

    virtual int packetId() const{
        return 6;
    }

    friend QDataStream &operator>>(QDataStream &data, CarTelemetryDataPacket *packet){
        for(auto x = 0; x < 20 ; x++)
            data >> &packet->m_carTelemetryData[x];

        data.readRawData((char *)&packet->m_buttonStatus, sizeof(quint32));


        return data;
    }
};


#endif // PACKETS_H
