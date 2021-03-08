// ======================================================================
// \title  SimpleMotorControlComponentImpl.hpp
// \author joe
// \brief  hpp file for SimpleMotorControl component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef SimpleMotorControl_HPP
#define SimpleMotorControl_HPP

#include "Tank/SimpleMotorControl/SimpleMotorControlComponentAc.hpp"

namespace Tank {

  class SimpleMotorControlComponentImpl :
    public SimpleMotorControlComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object SimpleMotorControl
      //!
      SimpleMotorControlComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object SimpleMotorControl
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object SimpleMotorControl
      //!
      ~SimpleMotorControlComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for MotorDirection
      //!
      void MotorDirection_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          MotorDirection direction /*!< Motor Direction Control*/
      );

      //! Handler implementation for MotorEnable
      //!
      void MotorEnable_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          bool state 
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for TankMotor_Enable command handler
      //! Command to Enable or Disable a Motor
      void TankMotor_Enable_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          bool enable /*!< Enable or Disable the Motor*/
      );

      //! Implementation for TankMotor_SetDirection command handler
      //! Command to set a motor direction
      void TankMotor_SetDirection_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          Tank::MotorDirection direction /*!< The motor direction*/
      );


    };

} // end namespace Tank

#endif
