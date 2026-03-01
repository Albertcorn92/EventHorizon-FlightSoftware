// ======================================================================
// \title  Main.cpp
// ======================================================================
#include <GncDeployment/Top/GncDeploymentTopology.hpp>
#include <Drv/LinuxUartDriver/LinuxUartDriver.hpp>
#include <Os/Os.hpp>
#include <signal.h>
#include <getopt.h>
#include <cstdlib>

void print_usage(const char* app) {
    (void)printf("Usage: ./%s [options]\n-a\thostname/IP\n-p\tport\n-d\tuart device\n", app);
}

static void signalHandler(int signum) {
    GncDeployment::stopRateGroups();
}

int main(int argc, char* argv[]) {
    I32   option     = 0;
    CHAR* hostname   = nullptr;
    CHAR* uartDevice = nullptr;
    U16   port       = 0;

    Os::init();

    while ((option = getopt(argc, argv, "hp:a:d:")) != -1) {
        switch (option) {
            case 'a': hostname   = optarg; break;
            case 'p': port       = static_cast<U16>(atoi(optarg)); break;
            case 'd': uartDevice = optarg; break;
            case 'h':
            case '?':
            default:
                print_usage(argv[0]);
                return (option == 'h') ? 0 : 1;
        }
    }

    GncDeployment::TopologyState inputs;
    inputs.hostname   = hostname;
    inputs.port       = port;
    inputs.uartDevice = uartDevice;
    inputs.uartBaud   = Drv::LinuxUartDriver::BAUD_115K;

    signal(SIGINT,  signalHandler);
    signal(SIGTERM, signalHandler);
    (void)printf("Hit Ctrl-C to quit\n");

    GncDeployment::setupTopology(inputs);
    GncDeployment::startRateGroups(Fw::TimeInterval(1, 0));
    GncDeployment::teardownTopology(inputs);
    (void)printf("Exiting...\n");
    return 0;
}