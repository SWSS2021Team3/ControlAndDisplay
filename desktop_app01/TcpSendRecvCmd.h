#pragma once

#include "NetworkTCP.h"
#include "Payload.h"
#include "serialize.h"

int TcpSendCommand(TTcpConnectedPort * TcpConnectedPort, Payload* payload);
bool TcpRecvCommand(TTcpConnectedPort * TcpConnectedPort, Payload* payload);

int TcpSendObject(TTcpConnectedPort* TcpConnectedPort, Serializable* s);
bool TcpRecvObject(TTcpConnectedPort* TcpConnectedPort, Serializable* s);
