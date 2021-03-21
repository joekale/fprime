// ======================================================================
// \title  PWMMotorControlComponentImpl.cpp
// \author joe
// \brief  cpp file for PWMMotorControl component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Tank/PWMMotorControl/PWMMotorControlComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Tank {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  PWMMotorControlComponentImpl ::
    PWMMotorControlComponentImpl(
        const char *const compName
    ) : PWMMotorControlComponentBase(compName)
  {

  }

  void PWMMotorControlComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    PWMMotorControlComponentBase::init(instance);
  }

  PWMMotorControlComponentImpl ::
    ~PWMMotorControlComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void PWMMotorControlComponentImpl ::
    MotorDirection_handler(
        const NATIVE_INT_TYPE portNum,
        Tank::MotorDirection direction
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

  void PWMMotorControlComponentImpl ::
    MotorSpeed_handler(
        const NATIVE_INT_TYPE portNum,
        F32 speed
    )
  {
    this->motorSpeed_out(0, speed);
    this->tlmWrite_TMSC_MotorSpeedChange(speed);
    this->log_DIAGNOSTIC_TSMC_MotorSpeedChange(speed);
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void PWMMotorControlComponentImpl ::
    TankMotor_Speed_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        F32 speed
    )
  {
    this->MotorSpeed_handler(0, speed);
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void PWMMotorControlComponentImpl ::
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
