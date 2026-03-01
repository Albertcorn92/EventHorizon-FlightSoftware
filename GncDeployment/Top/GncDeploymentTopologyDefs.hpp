// ======================================================================
// \title  GncDeploymentTopologyDefs.hpp
// ======================================================================
#ifndef GNCDEPLOYMENT_GNCDEPLOYMENTTOPOLOGYDEFS_HPP
#define GNCDEPLOYMENT_GNCDEPLOYMENTTOPOLOGYDEFS_HPP

#include "Svc/Subtopologies/CdhCore/PingEntries.hpp"
#include "Svc/Subtopologies/ComCcsds/PingEntries.hpp"
#include "Svc/Subtopologies/DataProducts/PingEntries.hpp"
#include "Svc/Subtopologies/FileHandling/PingEntries.hpp"
#include "Svc/Subtopologies/CdhCore/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/ComCcsds/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/DataProducts/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/FileHandling/SubtopologyTopologyDefs.hpp"
#include "Svc/Subtopologies/ComCcsds/Ports_ComPacketQueueEnumAc.hpp"
#include "Svc/Subtopologies/ComCcsds/Ports_ComBufferQueueEnumAc.hpp"
#include "GncDeployment/Top/FppConstantsAc.hpp"
#include "Drv/LinuxUartDriver/LinuxUartDriver.hpp"

namespace PingEntries {
    namespace GncDeployment_rateGroup1 {enum { WARN = 3, FATAL = 5 };}
    namespace GncDeployment_rateGroup2 {enum { WARN = 3, FATAL = 5 };}
    namespace GncDeployment_rateGroup3 {enum { WARN = 3, FATAL = 5 };}
    namespace GncDeployment_cmdSeq {enum { WARN = 3, FATAL = 5 };}
}

namespace GncDeployment {

struct TopologyState {
    const char* hostname;
    U16 port;
    const char* uartDevice;
    Drv::LinuxUartDriver::UartBaudRate uartBaud;
    CdhCore::SubtopologyState cdhCore;
    ComCcsds::SubtopologyState comCcsds;
    DataProducts::SubtopologyState dataProducts;
    FileHandling::SubtopologyState fileHandling;
};

namespace PingEntries = ::PingEntries;
}

#endif