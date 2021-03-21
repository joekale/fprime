// ======================================================================
// \title  LinuxPWMDriverComponentImpl.hpp
// \author joe
// \brief  hpp file for LinuxPWMDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef LinuxPWMDriver_HPP
#define LinuxPWMDriver_HPP

#include "Tank/LinuxPWMDriver/LinuxPWMDriverComponentAc.hpp"

#define DEFAULT_PERIOD 10000000

namespace Tank {

  class LinuxPWMDriverComponentImpl :
    public LinuxPWMDriverComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------


      //! configure GPIO
      enum PWMPolarity {
          PWM_NORMAL, //!< normal
          PWM_INVERSE, //!< inverse
      };
      
      //! Construct object LinuxPWMDriver
      //!
      LinuxPWMDriverComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object LinuxPWMDriver
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! open GPIO
      bool open(NATIVE_INT_TYPE pwm, PWMPolarity polarity);

      //! Destroy object LinuxPWMDriver
      //!
      ~LinuxPWMDriverComponentImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for dutyCycleWrite
      //!
      void dutyCycleWrite_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          F32 duty_cycle /*!< percent of period to set duty cycle between 0 and 1*/
      );

      U32 period;
      PWMPolarity polarity;
      NATIVE_INT_TYPE m_fd;
      NATIVE_INT_TYPE m_pwm;
    };

} // end namespace Tank

#endif
