// ======================================================================
// \title  SimpleMotorControlComponentImpl.cpp
// \author joe
// \brief  cpp file for SimpleMotorControl component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Tank/SimpleMotorControl/SimpleMotorControlComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Tank {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  SimpleMotorControlComponentImpl ::
    SimpleMotorControlComponentImpl(
        const char *const compName
    ) : SimpleMotorControlComponentBase(compName)
  {

  }

  void SimpleMotorControlComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    SimpleMotorControlComponentBase::init(instance);
  }

  SimpleMotorControlComponentImpl ::
    ~SimpleMotorControlComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void SimpleMotorControlComponentImpl ::
    MotorDirection_handler(
        const NATIVE_INT_TYPE portNum,
        MotorDirection direction
    )
  {
    switch (direction.e) {
      case MotorDirection::NONE:
        this->motorControl0_out(0, 0);
        this->motorControl1_out(0, 0);
        break;
      case MotorDirection::FORWARD:
        this->motorControl0_out(0, 1);
        this->motorControl1_out(0, 0);
        break;
      case MotorDirection::REVERSE:
        this->motorControl0_out(0, 0);
        this->motorControl1_out(0, 1);
        break;
      case MotorDirection::BRAKE:
        this->motorControl0_out(0, 1);
        this->motorControl1_out(0, 1);
        break;
      default:
        FW_ASSERT(0,direction.e);
        break;
    }

    this->tlmWrite_TMSC_Direction(direction);
    this->log_DIAGNOSTIC_TSMC_MotorDirectionChange(direction);
  }

  void SimpleMotorControlComponentImpl ::
    MotorEnable_handler(
        const NATIVE_INT_TYPE portNum,
        bool state
    )
  {
    this->motorEnable_out(0, state);
    this->tlmWrite_TMSC_Enabled(state);
    Fw::LogStringArg val = (state) ? "Enabled" : "Disabled";
    this->log_ACTIVITY_HI_TSMC_MotorEnableChange(val);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void SimpleMotorControlComponentImpl ::
    TankMotor_Enable_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        bool enable
    )
  {
    this->MotorEnable_handler(0, enable);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void SimpleMotorControlComponentImpl ::
    TankMotor_SetDirection_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        Tank::MotorDirection direction
    )
  {
    this->MotorDirection_handler(0, direction);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace Tank
