#include "ProximityDriver/ProximityDriver.hpp"
#include <Fw/Logger/Logger.hpp>
#include <cstring>
#include <cstdio>
#include <cmath>

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

  float ProximityDriver ::
    constrain(float val, float minVal, float maxVal)
  {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
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
      // Emit raw sensor data
      this->tlmWrite_Z_Distance(dist);
      this->tlmWrite_Signal_Strength(sig);
      this->tlmWrite_X_Error(errX);
      this->tlmWrite_Y_Error(errY);

      // Thruster Calculation Logic (Mapped from v2.1 Sketch)
      const float DEADZONE = 20.0f;
      U32 tTop = 0, tBtm = 0, tLft = 0, tRgt = 0;

      // Y-Axis Logic
      if (std::abs(errY) > DEADZONE) {
          float pwr = (std::abs(errY) - DEADZONE) * (90.0f / (250.0f - DEADZONE)) + 10.0f;
          U32 thrustPwr = static_cast<U32>(constrain(pwr, 10.0f, 100.0f));
          if (errY < -DEADZONE) {
              tTop = thrustPwr;
          } else {
              tBtm = thrustPwr;
          }
      }

      // X-Axis Logic
      if (std::abs(errX) > DEADZONE) {
          float pwr = (std::abs(errX) - DEADZONE) * (90.0f / (250.0f - DEADZONE)) + 10.0f;
          U32 thrustPwr = static_cast<U32>(constrain(pwr, 10.0f, 100.0f));
          if (errX < -DEADZONE) {
              tLft = thrustPwr;
          } else {
              tRgt = thrustPwr;
          }
      }

      // Emit Thruster Telemetry
      this->tlmWrite_Thrust_Top(tTop);
      this->tlmWrite_Thrust_Bottom(tBtm);
      this->tlmWrite_Thrust_Left(tLft);
      this->tlmWrite_Thrust_Right(tRgt);
    }
  }
}