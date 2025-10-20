
#pragma once
#include "Observer.h"
#include "SinglePortModule.h"

/**
 * Text message handling for meshtastic - draws on the OLED display the most recent received message
 */
class CustomModule : public SinglePortModule, public Observable<const meshtastic_MeshPacket *>
{
  public:
    /** Constructor
     * name is for debugging output
     */
    CustomModule() : SinglePortModule("text", meshtastic_PortNum_TEXT_MESSAGE_APP)
    {
      availablePins = (1 << 17) | (1 << 22) | (1 << 2); 
      for (int pin = 0; pin < 32; pin++)
      {
        if ((availablePins >> pin) & 1)
        {
          pinMode(pin, OUTPUT);
          digitalWrite(pin, 1);
        }
      }
    }


  protected:
    /** Called to handle a particular incoming message

    @return ProcessMessage::STOP if you've guaranteed you've handled this message and no other handlers should be considered for
    it
    */

    /// A bitmask of GPIOs that are exposed to the mesh if undefined access is not enabled

    virtual ProcessMessage handleReceived(const meshtastic_MeshPacket &mp) override;
    virtual bool wantPacket(const meshtastic_MeshPacket *p) override;
    virtual meshtastic_MeshPacket *allocReply() override;
  
  private:
    uint64_t availablePins;

    void sendReply(const char *, const meshtastic_MeshPacket&);
};

extern CustomModule *customModule;
