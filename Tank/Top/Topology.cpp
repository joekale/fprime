#include <Components.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Task.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Os/Log.hpp>
#include <Fw/Types/MallocAllocator.hpp>

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#endif

// List of context IDs
enum {
    DOWNLINK_PACKET_SIZE = 500,
    DOWNLINK_BUFFER_STORE_SIZE = 2500,
    DOWNLINK_BUFFER_QUEUE_SIZE = 5,
    UPLINK_BUFFER_STORE_SIZE = 3000,
    UPLINK_BUFFER_QUEUE_SIZE = 30
};

Os::Log osLogger;


// Registry
#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

// Component instance pointers
static NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriverImpl::DIVIDER_SIZE] = {1,2,4};
Svc::RateGroupDriverImpl rateGroupDriverComp(FW_OPTIONAL_NAME("RGDvr"),rgDivs,FW_NUM_ARRAY_ELEMENTS(rgDivs));

static NATIVE_UINT_TYPE rg1Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup1Comp(FW_OPTIONAL_NAME("RG1"),rg1Context,FW_NUM_ARRAY_ELEMENTS(rg1Context));

static NATIVE_UINT_TYPE rg2Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup2Comp(FW_OPTIONAL_NAME("RG2"),rg2Context,FW_NUM_ARRAY_ELEMENTS(rg2Context));

static NATIVE_UINT_TYPE rg3Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup3Comp(FW_OPTIONAL_NAME("RG3"),rg3Context,FW_NUM_ARRAY_ELEMENTS(rg3Context));

// Command Components
Svc::GroundInterfaceComponentImpl groundIf(FW_OPTIONAL_NAME("GNDIF"));

#if FW_ENABLE_TEXT_LOGGING
Svc::ConsoleTextLoggerImpl textLogger(FW_OPTIONAL_NAME("TLOG"));
#endif

Svc::ActiveLoggerImpl eventLogger(FW_OPTIONAL_NAME("ELOG"));

Svc::LinuxTimeImpl linuxTime(FW_OPTIONAL_NAME("LTIME"));
Svc::LinuxTimerComponentImpl linuxTimer(FW_OPTIONAL_NAME("LINUXTIMER"));

Svc::TlmChanImpl chanTlm(FW_OPTIONAL_NAME("TLM"));

Svc::CommandDispatcherImpl cmdDisp(FW_OPTIONAL_NAME("CMDDISP"));

Fw::MallocAllocator seqMallocator;
Svc::CmdSequencerComponentImpl cmdSeq(FW_OPTIONAL_NAME("CMDSEQ"));

Svc::PrmDbImpl prmDb(FW_OPTIONAL_NAME("PRM"),"PrmDb.dat");

Drv::SocketIpDriverComponentImpl socketIpDriver(FW_OPTIONAL_NAME("SocketIpDriver"));

Svc::FileUplink fileUplink(FW_OPTIONAL_NAME("fileUplink"));

Svc::FileDownlink fileDownlink(FW_OPTIONAL_NAME("fileDownlink"));

Svc::BufferManager fileUplinkBufferManager(FW_OPTIONAL_NAME("fileUplinkBufferManager"), UPLINK_BUFFER_STORE_SIZE, UPLINK_BUFFER_QUEUE_SIZE);

Svc::HealthImpl health(FW_OPTIONAL_NAME("health"));

// Tank Implementation Components
Tank::PWMMotorControlComponentImpl motorControlR(FW_OPTIONAL_NAME("motorControlR"));
Tank::PWMMotorControlComponentImpl motorControlL(FW_OPTIONAL_NAME("motorControlL"));
Tank::LinuxPWMDriverComponentImpl pwmR(FW_OPTIONAL_NAME("pwmR"));
Drv::LinuxGpioDriverComponentImpl gpioR1(FW_OPTIONAL_NAME("gpioR1"));
Drv::LinuxGpioDriverComponentImpl gpioR2(FW_OPTIONAL_NAME("gpioR2"));
Tank::LinuxPWMDriverComponentImpl pwmL(FW_OPTIONAL_NAME("pwmL"));
Drv::LinuxGpioDriverComponentImpl gpioL1(FW_OPTIONAL_NAME("gpioL1"));
Drv::LinuxGpioDriverComponentImpl gpioL2(FW_OPTIONAL_NAME("gpioL2"));

Svc::AssertFatalAdapterComponentImpl fatalAdapter(FW_OPTIONAL_NAME("fatalAdapter"));

Svc::FatalHandlerComponentImpl fatalHandler(FW_OPTIONAL_NAME("fatalHandler"));

const char* getHealthName(Fw::ObjBase& comp) {
   #if FW_OBJECT_NAMES == 1
       return comp.getObjName();
   #else
      return "[no object name]"
   #endif
}

bool constructApp(bool dump, U32 port_number, char* hostname) {

#if FW_PORT_TRACING
    Fw::PortBase::setTrace(false);
#endif    

    // Initialize rate group driver
    rateGroupDriverComp.init();

    // Initialize the rate groups
    rateGroup1Comp.init(10,0);
    
    rateGroup2Comp.init(10,1);
    
    rateGroup3Comp.init(10,2);

#if FW_ENABLE_TEXT_LOGGING
    textLogger.init();
#endif

    eventLogger.init(10,0);
    
    linuxTime.init(0);

    linuxTimer.init(0);

    chanTlm.init(10,0);

    cmdDisp.init(20,0);

    cmdSeq.init(10,0);
    cmdSeq.allocateBuffer(0,seqMallocator,5*1024);

    prmDb.init(10,0);

    groundIf.init(0);
    socketIpDriver.init(0);

    fileUplink.init(30, 0);
    fileDownlink.init(30, 0);
    fileDownlink.configure(1000, 1000, 1000, 10);
    fileUplinkBufferManager.init(0);
    fatalAdapter.init(0);
    fatalHandler.init(0);
    health.init(25,0);

    motorControlR.init(0);
    motorControlL.init(0);
    pwmR.init(0);
    gpioR1.init(0);
    gpioR2.init(0);
    pwmL.init(0);
    gpioL1.init(0);
    gpioL2.init(0);
    // Connect rate groups to rate group driver
    constructTankArchitecture();

    // dump topology if requested
    if (dump) {
#if FW_OBJECT_REGISTRATION == 1
        simpleReg.dump();
#endif
        return true;
    }

    /* Register commands */
    cmdSeq.regCommands();
    cmdDisp.regCommands();
    eventLogger.regCommands();
    prmDb.regCommands();
    fileDownlink.regCommands();
    health.regCommands();

    motorControlR.regCommands();
    motorControlL.regCommands();

    // read parameters
    prmDb.readParamFile();

    // set health ping entries

    Svc::HealthImpl::PingEntry pingEntries[] = {
        {3,5,getHealthName(rateGroup1Comp)}, // 0
        {3,5,getHealthName(rateGroup2Comp)}, // 1
        {3,5,getHealthName(rateGroup3Comp)}, // 2
        {3,5,getHealthName(cmdDisp)}, // 3
        {3,5,getHealthName(eventLogger)}, // 4
        {3,5,getHealthName(cmdSeq)}, // 5
        {3,5,getHealthName(chanTlm)}, // 6
        {3,5,getHealthName(prmDb)}, // 7
        {3,5,getHealthName(fileUplink)}, // 8
        {3,5,getHealthName(fileDownlink)}, // 9]
    };

    // register ping table
    health.setPingEntries(pingEntries,FW_NUM_ARRAY_ELEMENTS(pingEntries),0x123);

    // Active component startup
    // start rate groups
    rateGroup1Comp.start(0, 120,10 * 1024);
    rateGroup2Comp.start(0, 119,10 * 1024);
    rateGroup3Comp.start(0, 118,10 * 1024);
    // start dispatcher
    cmdDisp.start(0,101,10*1024);
    // start sequencer
    cmdSeq.start(0,100,10*1024);
    // start telemetry
    eventLogger.start(0,98,10*1024);
    chanTlm.start(0,97,10*1024);
    prmDb.start(0,96,10*1024);

    fileDownlink.start(0, 100, 10*1024);
    fileUplink.start(0, 100, 10*1024);

    // Initialize socket server if and only if there is a valid specification
    if (hostname != NULL && port_number != 0) {
        socketIpDriver.startSocketTask(100, 10 * 1024, hostname, port_number);
    }

    if (not pwmR.open(0,Tank::LinuxPWMDriverComponentImpl::PWM_NORMAL)) {
        return true;
    }

    if (not gpioR1.open(27,Drv::LinuxGpioDriverComponentImpl::GPIO_OUT)) {
        return true;
    }

    if (not gpioR2.open(22,Drv::LinuxGpioDriverComponentImpl::GPIO_OUT)) {
        return true;
    }

    if (not pwmL.open(1,Tank::LinuxPWMDriverComponentImpl::PWM_NORMAL)) {
        return true;
    }

    if (not gpioL1.open(24,Drv::LinuxGpioDriverComponentImpl::GPIO_OUT)) {
        return true;
    }

    if (not gpioL2.open(25,Drv::LinuxGpioDriverComponentImpl::GPIO_OUT)) {
        return true;
    }

    return false;
}

void exitTasks(void) {
    rateGroup1Comp.exit();
    rateGroup2Comp.exit();
    rateGroup3Comp.exit();
    cmdDisp.exit();
    eventLogger.exit();
    chanTlm.exit();
    prmDb.exit();
    fileUplink.exit();
    fileDownlink.exit();
    cmdSeq.exit();
    // join the component threads with NULL pointers to free them
    (void) rateGroup1Comp.ActiveComponentBase::join(NULL);
    (void) rateGroup2Comp.ActiveComponentBase::join(NULL);
    (void) rateGroup3Comp.ActiveComponentBase::join(NULL);
    (void) cmdDisp.ActiveComponentBase::join(NULL);
    (void) eventLogger.ActiveComponentBase::join(NULL);
    (void) chanTlm.ActiveComponentBase::join(NULL);
    (void) prmDb.ActiveComponentBase::join(NULL);
    (void) fileUplink.ActiveComponentBase::join(NULL);
    (void) fileDownlink.ActiveComponentBase::join(NULL);
    (void) cmdSeq.ActiveComponentBase::join(NULL);
    socketIpDriver.exitSocketTask();
    (void) socketIpDriver.joinSocketTask(NULL);
    cmdSeq.deallocateBuffer(seqMallocator);
}

