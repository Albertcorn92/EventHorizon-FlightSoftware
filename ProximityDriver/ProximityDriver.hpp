#ifndef GncDeployment_ProximityDriver_HPP
#define GncDeployment_ProximityDriver_HPP

#include "ProximityDriver/ProximityDriverComponentAc.hpp"

namespace GncDeployment {

  class ProximityDriver :
    public ProximityDriverComponentBase
  {
    public:
      ProximityDriver(const char* const compName);
      ~ProximityDriver();

    protected: // Changed from PRIVATE to protected to match standard F' patterns
      void ready_handler(FwIndexType portNum);

      void serialRecv_handler(
          FwIndexType portNum,
          Fw::Buffer& buffer,
          const Drv::ByteStreamStatus& status
      );

      void parseAndEmit(const char* line);

    private:
      char m_lineBuffer[256];
      U32 m_lineIndex;

      // Helper function for thruster calculation bounds
      float constrain(float val, float minVal, float maxVal);
  };
}
#endif