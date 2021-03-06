#pragma once
#ifndef _TCPSENDRECVCMD_H
#define _TCPSENDRECVCMD_H

#include "NetworkTCP.h"
#include "Payload.h"
#include "serialize.h"

int TcpSendCommand(TTcpConnectedPort* TcpConnectedPort, Payload* payload);
bool TcpRecvCommand(TTcpConnectedPort* TcpConnectedPort, Payload* payload);

int TcpSendObject(TTcpConnectedPort* TcpConnectedPort, Serializable* s);
bool TcpRecvObject(TTcpConnectedPort* TcpConnectedPort, Serializable* s);

int TcpSendStringVector(TTcpConnectedPort* TcpConnectedPort, vector<string>& s);
bool TcpRecvStringVector(TTcpConnectedPort* TcpConnectedPort, vector<string>& s);

#endif // _TCPSENDRECVCMD_H