#include "ProximityDriver/ProximityDriver.hpp"
#include <Fw/Logger/Logger.hpp>
#include <cstring>
#include <cstdio>

namespace GncDeployment {

  ProximityDriver ::
    ProximityDriver(const char* const compName) :
      ProximityDriverComponentBase(compName),
      m_lineIndex(0)
  {
    memset(m_lineBuffer, 0, sizeof(m_lineBuffer));
  }

  ProximityDriver ::
    ~ProximityDriver()
  {
  }

  void ProximityDriver ::
    ready_handler(FwIndexType portNum)
  {
    Fw::Logger::log("[PROX] Ready signal received\n");
  }

  void ProximityDriver ::
    serialRecv_handler(
        FwIndexType portNum,
        Fw::Buffer& buffer,
        const Drv::ByteStreamStatus& status
    )
  {
    // Always deallocate the buffer eventually, but handle status first
    if (status != Drv::ByteStreamStatus::OP_OK) {
      this->deallocate_out(0, buffer);
      return;
    }

    const U8* data = buffer.getData();
    U32 size = buffer.getSize();

    if (data != nullptr && size > 0) {
      for (U32 i = 0; i < size; i++) {
        char c = static_cast<char>(data[i]);
        if (c == '\n' || c == '\r') {
          if (m_lineIndex > 0) {
            m_lineBuffer[m_lineIndex] = '\0';
            parseAndEmit(m_lineBuffer);
          }
          m_lineIndex = 0;
        } else if (m_lineIndex < sizeof(m_lineBuffer) - 1) {
          if (c >= 32 && c <= 126) {
            m_lineBuffer[m_lineIndex++] = c;
          }
        } else {
          m_lineIndex = 0;
        }
      }
    }

    this->deallocate_out(0, buffer);
  }

  void ProximityDriver ::
    parseAndEmit(const char* line)
  {
    if (line == nullptr || strncmp(line, "$GNC,", 5) != 0) {
      return;
    }

    float dist = 0.0f, sig = 0.0f, errX = 0.0f, errY = 0.0f;
    int parsed = sscanf(line, "$GNC,%f,%f,%f,%f", &dist, &sig, &errX, &errY);

    if (parsed == 4) {
      this->tlmWrite_Z_Distance(dist);
      this->tlmWrite_Signal_Strength(sig);
      this->tlmWrite_X_Error(errX);
      this->tlmWrite_Y_Error(errY);
    }
  }
}