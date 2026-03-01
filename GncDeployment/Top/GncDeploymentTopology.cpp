// ======================================================================
// \title  GncDeploymentTopology.cpp
// ======================================================================
#include <GncDeployment/Top/GncDeploymentTopologyAc.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <cstring>

namespace GncDeployment {

Fw::MallocAllocator mallocator;
Svc::RateGroupDriver::DividerSet rateGroupDivisorsSet{{{1, 0}, {2, 0}, {4, 0}}};
U32 rateGroup1Context[Svc::ActiveRateGroup::CONNECTION_COUNT_MAX] = {};
U32 rateGroup2Context[Svc::ActiveRateGroup::CONNECTION_COUNT_MAX] = {};
U32 rateGroup3Context[Svc::ActiveRateGroup::CONNECTION_COUNT_MAX] = {};

enum TopologyConstants {
    COMM_PRIORITY = 34,
    UART_PRIORITY = 50,
};

void configureTopology() {
    rateGroupDriver.configure(rateGroupDivisorsSet);
    rateGroup1.configure(rateGroup1Context, FW_NUM_ARRAY_ELEMENTS(rateGroup1Context));
    rateGroup2.configure(rateGroup2Context, FW_NUM_ARRAY_ELEMENTS(rateGroup2Context));
    rateGroup3.configure(rateGroup3Context, FW_NUM_ARRAY_ELEMENTS(rateGroup3Context));
    cmdSeq.allocateBuffer(0, mallocator, 5 * 1024);

    // --- UART BUFFER MANAGER SETUP ---
    Svc::BufferManager::BufferBins uartBins;
    memset(&uartBins, 0, sizeof(uartBins));
    
    // Set bin size to 4096 to safely accommodate the 2048 UART requests
    uartBins.bins[0].bufferSize  = 4096;
    uartBins.bins[0].numBuffers  = 20;
    
    U32 totalSize = uartBins.bins[0].bufferSize * uartBins.bins[0].numBuffers;
    
    // setup(ID, totalSize, allocator, binConfig)
    uartBufferManager.setup(0, totalSize, mallocator, uartBins);
}

void setupTopology(const TopologyState& state) {
    initComponents(state);
    setBaseIds();
    connectComponents();
    regCommands();
    configComponents(state);

    if (state.hostname != nullptr && state.port != 0) {
        comDriver.configure(state.hostname, state.port);
    }

    // CRITICAL: Initialize BufferManager memory BEFORE starting the UART thread.
    // This prevents the "Line 104" crash during startup.
    configureTopology();

    // Configure and open UART for ESP32 sensor data
    if (state.uartDevice != nullptr) {
        // We open the driver with 2048, which fits inside our 4096 bins
        uartDriver.open(state.uartDevice,
                        state.uartBaud,
                        Drv::LinuxUartDriver::NO_FLOW,
                        Drv::LinuxUartDriver::PARITY_NONE,
                        2048); 
        
        // Start the read thread now that memory is allocated
        uartDriver.start(UART_PRIORITY, Default::STACK_SIZE);
    }

    loadParameters();
    startTasks(state);

    if (state.hostname != nullptr && state.port != 0) {
        Os::TaskString name("ReceiveTask");
        comDriver.start(name, COMM_PRIORITY, Default::STACK_SIZE);
    }
}

void startRateGroups(const Fw::TimeInterval& interval) {
    timer.startTimer(interval);
}

void stopRateGroups() {
    timer.quit();
}

void teardownTopology(const TopologyState& state) {
    stopTasks(state);
    freeThreads(state);
    comDriver.stop();
    (void)comDriver.join();
    uartDriver.quitReadThread();
    cmdSeq.deallocateBuffer(mallocator);
    uartBufferManager.cleanup();
    tearDownComponents(state);
}

} // end namespace GncDeployment