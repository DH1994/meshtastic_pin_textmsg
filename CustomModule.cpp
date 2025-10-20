#include "CustomModule.h"
#include "MeshService.h"
#include "NodeDB.h"
#include "PowerFSM.h"
#include "buzz.h"
#include "configuration.h"
#include <cstdio>
#include <regex>


CustomModule *customModule;

ProcessMessage CustomModule::handleReceived(const meshtastic_MeshPacket &mp)
{
    // #ifdef DEBUG_PORT
    // meshtastic_MeshPacket &p = mp.decoded;
    // LOG_INFO("Received text msg from=0x%0x, id=0x%x, msg=%.*s", mp.from, mp.id, p.payload.size, p.payload.bytes);
    // #endif

    std::string msg = reinterpret_cast<const char *>(mp.decoded.payload.bytes);

    std::regex pattern(R"(^(\w+)\s+(\d+)\s+(\d+)$)");
    std::smatch matches;

    if (std::regex_match(msg, matches, pattern)) {
        std::string cmd = matches[1];
        int pin = std::stoi(matches[2]);
        int val = std::stoi(matches[3]);

        if (cmd == "pin") {
            if ((availablePins >> pin) & 1)
            {
                digitalWrite(pin, val);

                const char *reply = val ? "PIN ON" : "PIN OFF";
                sendReply(reply, mp);
            }
            else
            {
                sendReply("Pin not available", mp);
            }
        }
    }

    return ProcessMessage::CONTINUE; // Let others look at this message also if they want
}

bool CustomModule::wantPacket(const meshtastic_MeshPacket *p)
{
    return MeshService::isTextPayload(p);
}

meshtastic_MeshPacket *CustomModule::allocReply()
{
    auto reply = allocDataPacket(); // Allocate a packet for sending

    return reply;
}

/**
 * Sends a payload to a specified destination node.
 *
 * @param dest The destination node number.
 * @param wantReplies Whether or not to request replies from the destination node.
 */
void CustomModule::sendReply(const char * text, const meshtastic_MeshPacket &req)
{
    const meshtastic_Channel *ch = (boundChannel != NULL) ? &channels.getByName(boundChannel) : NULL;
    meshtastic_MeshPacket *p = allocReply();

    setReplyTo(p, req);
    if (ch != NULL) {
        p->channel = ch->index;
    }
    p->decoded.want_response = false;

    p->want_ack = true;

    p->decoded.payload.size = strlen(text); // You must specify how many bytes are in the reply
    memcpy(p->decoded.payload.bytes, text, p->decoded.payload.size);

    service->sendToMesh(p);
}