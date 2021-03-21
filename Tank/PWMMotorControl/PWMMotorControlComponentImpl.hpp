// ======================================================================
// \title  PWMMotorControlComponentImpl.hpp
// \author joe
// \brief  hpp file for PWMMotorControl component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef PWMMotorControl_HPP
#define PWMMotorControl_HPP

#include "Tank/PWMMotorControl/PWMMotorControlComponentAc.hpp"

namespace Tank {

  class PWMMotorControlComponentImpl :
    public PWMMotorControlComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object PWMMotorControl
      //!
      PWMMotorControlComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object PWMMotorControl
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object PWMMotorControl
      //!
      ~PWMMotorControlComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for MotorDirection
      //!
      void MotorDirection_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Tank::MotorDirection direction /*!< Motor Direction Control*/
      );

      //! Handler implementation for MotorSpeed
      //!
      void MotorSpeed_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          F32 speed /*!< Percentage of max Motor Speed between 0 and 1*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for TankMotor_Speed command handler
      //! Command to Change Speed of a Motor
      void TankMotor_Speed_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          F32 speed /*!< Speed percent between 0 and 1*/
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
