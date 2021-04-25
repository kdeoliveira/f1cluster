#ifndef LISTENER_H
#define LISTENER_H

#include <QObject>
#include <QtNetwork>
#include "Packets.h"

class Listener : public QObject
{
    Q_OBJECT
public:
    explicit Listener(QObject *parent = nullptr, quint16 portNumber = defaultPort);
    ~Listener();
    static Listener* instance(); //Singleton

    void test();


    PacketSerial packetSerial() const;

signals:
    void packetRead();
private slots:
    void processDatagrams();
    void read();

private:
    static Listener* m_instance;
    quint16 m_port;
    static const quint16 defaultPort = 20777;
    QUdpSocket* socket;
    QByteArray buffer;
    quint64 nextBufferSize;
    bool connected;

    PacketHeader* m_packetHeader;
    PacketSerial* m_packetSerial;
    PacketInterface* m_packetData;
};

#endif // LISTENER_H
