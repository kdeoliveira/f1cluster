#include "listener.h"

Listener* Listener::m_instance = 0;

Listener::Listener(QObject *parent, quint16 portNumber) : QObject(parent), m_port(portNumber), socket(new QUdpSocket), nextBufferSize(PacketSize::header), connected(false), m_packetHeader(new PacketHeader), m_packetSerial(new PacketSerial)
{
    socket->bind(QHostAddress::Any, m_port, QUdpSocket::ShareAddress);

    if(socket->state() == QUdpSocket::ConnectedState)
        connected = true;
    connect(socket, &QUdpSocket::readyRead, this, &Listener::processDatagrams);

}

Listener* Listener::instance(){
    if(!m_instance)
        m_instance = new Listener;
    return m_instance;
}

void Listener::test()
{
    QByteArray temp("28041830A0A41830A0A");
    temp >> m_packetSerial;

    qDebug() << m_packetSerial->toBytes();
}

void Listener::processDatagrams()
{
    while(socket->hasPendingDatagrams())
    {
        buffer.resize(socket->pendingDatagramSize());
        socket->readDatagram(buffer.data(), 2048);
        this->read();
    }
}

void Listener::read()
{

    if(buffer.count() < (int) nextBufferSize) return;
    QByteArray bufferArray = buffer.left(nextBufferSize);
    QDataStream stream(&bufferArray, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    buffer.remove(0, nextBufferSize);

    switch(nextBufferSize){
        case PacketSize::header:
            stream >> m_packetHeader;
            qDebug() << "{\n" << "PacketFormat: " << m_packetHeader->m_packetFormat << "\n Packet ID: " <<m_packetHeader->m_packetId << "\n}";
            break;
        case PacketSize::carTelemetry:
            stream >> reinterpret_cast<CarTelemetryDataPacket*>(m_packetData);
            qDebug() << "{\n" << "Engine Sped: " << ((CarTelemetryDataPacket*)m_packetData)->m_carTelemetryData[0].m_speed << "\n Engine RPM: " << ((CarTelemetryDataPacket*)m_packetData)->m_carTelemetryData[0].m_engineRPM << "\n}";
            break;
        default:
            break;
    }

    if(nextBufferSize == PacketSize::header){
        try{
            nextBufferSize = _packetMap.at(m_packetHeader->m_packetId);
        }
        catch(...){
            nextBufferSize = PacketSize::header;
        }
    }else{
        nextBufferSize = PacketSize::header;
    }

    emit packetRead();

    return read();
}

PacketSerial Listener::packetSerial() const
{
    return *m_packetSerial;
}

Listener::~Listener(){
    if(socket->isOpen())
        socket->abort();
}

