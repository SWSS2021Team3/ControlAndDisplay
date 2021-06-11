#pragma once

#include "NetworkTCP.h"
#include "Payload.h"

int TcpSendCommand(TTcpConnectedPort * TcpConnectedPort, Payload* payload);
bool TcpRecvCommand(TTcpConnectedPort * TcpConnectedPort, Payload* payload);

