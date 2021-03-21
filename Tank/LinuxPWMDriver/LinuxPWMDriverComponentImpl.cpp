// ======================================================================
// \title  LinuxPWMDriverComponentImpl.cpp
// \author joe
// \brief  cpp file for LinuxPWMDriver component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Tank/LinuxPWMDriver/LinuxPWMDriverComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

// TODO make proper static constants for these
#define SYSFS_PWM_DIR "/sys/class/pwm/pwmchip0"
#define MAX_BUF 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <math.h>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Tank {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LinuxPWMDriverComponentImpl ::
    LinuxPWMDriverComponentImpl(
        const char *const compName
    ) : LinuxPWMDriverComponentBase(compName),
        period(DEFAULT_PERIOD), 
        polarity(PWM_NORMAL),
        m_fd(-1),
        m_pwm(-1)
  {

  }

  void LinuxPWMDriverComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    LinuxPWMDriverComponentBase::init(instance);
  }

  LinuxPWMDriverComponentImpl ::
    ~LinuxPWMDriverComponentImpl(void)
  {

  }

  // Code modified from linuxgpiodriver
    /****************************************************************
    * pwm_export
    ****************************************************************/
    int pwm_export(unsigned int pwm)
    {
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_PWM_DIR "/export", O_WRONLY);
        if (fd < 0) {
            DEBUG_PRINT("pwm/export error!\n");
            return -1;
        }

        // TODO check value of len
        len = snprintf(buf, sizeof(buf), "%d", pwm);
        if(write(fd, buf, len) != len) {
            (void) close(fd);
            DEBUG_PRINT("pwm/export error!\n");
            return -1;
        }
        (void) close(fd);

	/* NOTE(mereweth) - this is to allow systemd udev to make
	 * necessary filesystem changes after exporting
	 */
	usleep(100 * 1000);
	
        return 0;
    }

    /****************************************************************
     * pwm_unexport
     ****************************************************************/
    int pwm_unexport(unsigned int pwm)
    {
        int fd, len;
        char buf[MAX_BUF];

        fd = open(SYSFS_PWM_DIR "/unexport", O_WRONLY);
        if (fd < 0) {
            DEBUG_PRINT("pwm/unexport error!\n");
            return -1;
        }

        // TODO check value of len
        len = snprintf(buf, sizeof(buf), "%d", pwm);
        if(write(fd, buf, len) != len) {
            (void) close(fd);
            DEBUG_PRINT("pwm/unexport error!\n");
            return -1;
        }
        (void) close(fd);

	/* NOTE(mereweth) - this is to allow systemd udev to make
	 * necessary filesystem changes after unexporting
	 */
	usleep(100 * 1000);
	
        return 0;
    }

    /****************************************************************
     * pwm_set_polarity
     ****************************************************************/
    int pwm_set_polarity(unsigned int pwm, unsigned int polarity)
    {
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_PWM_DIR  "/pwm%d/polarity", pwm);
        FW_ASSERT(len > 0, len);

        fd = open(buf, O_WRONLY);
        if (fd < 0) {
            DEBUG_PRINT("pwm/polarity open error!\n");
            return -1;
        }

        // 0 is normal 1 is inverse
        const char *dir = !polarity ? "normal" : "inverse";
        len = strlen(dir);

        if (write(fd, dir, len) != len) {
            (void) close(fd);
            DEBUG_PRINT("pwm/polarity set error!\n");
            return -1;
        }

        (void) close(fd);
        return 0;
    }

    /****************************************************************
     * pwm_set_period
     ****************************************************************/
    int pwm_set_period(unsigned int pwm, unsigned int period)
    {
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_PWM_DIR  "/pwm%d/period", pwm);
        FW_ASSERT(len > 0, len);

        fd = open(buf, O_WRONLY);
        if (fd < 0) {
            DEBUG_PRINT("pwm/period error!\n");
            return -1;
        }

        // TODO check value of len
        len = snprintf(buf, sizeof(buf), "%d", period);
        if(write(fd, buf, len) != len) {
            (void) close(fd);
            DEBUG_PRINT("pwm/period error!\n");
            return -1;
        }

        (void) close(fd);
        return 0;
    }

    /****************************************************************
     * gpio_set_value(polarity == PWMPolarity::PWM_NORMAL)
     ****************************************************************/
    int pwm_set_duty_cycle(int fd, unsigned int duty_cycle)
    {

        int len;
        char buf[MAX_BUF];

        // TODO check value of len
        len = snprintf(buf, sizeof(buf), "%d", duty_cycle);
        if(write(fd, buf, len) != len) {
            (void) close(fd);
            DEBUG_PRINT("pwm/duty_cycle error!\n");
            return -1;
        }

        DEBUG_PRINT("PWM fd %d value %d written\n",fd,duty_cycle);
        return 0;
    }

    /****************************************************************
     * pwm_set_enable
     ****************************************************************/
    int pwm_set_enable(unsigned int pwm, unsigned int enable)
    {
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_PWM_DIR  "/pwm%d/enable", pwm);
        FW_ASSERT(len > 0, len);

        fd = open(buf, O_WRONLY);
        if (fd < 0) {
            DEBUG_PRINT("pwm/enable error!\n");
            return -1;
        }

        // TODO make value a enum or check its value

        const char *val = enable ? "1" : "0";
        len = 1;

        if(write(fd, val, len) != len) {
            (void) close(fd);
            DEBUG_PRINT("pwm/enable error!\n");
            return -1;
        }

        (void) close(fd);
        return 0;
    }

    /****************************************************************
     * gpio_get_value
     ****************************************************************/
    // int gpio_get_value(int fd, unsigned int *value)
    // {
    //     char ch = '0';

    //     FW_ASSERT(fd != -1);

    //     NATIVE_INT_TYPE stat1 = lseek(fd, 0, SEEK_SET); // Must seek back to the starting
    //     NATIVE_INT_TYPE stat2 = read(fd, &ch, 1);

    //     if (stat1 == -1 || stat2 != 1) {
    //         DEBUG_PRINT("GPIO read failure: %d %d!\n",stat1,stat2);
    //         return -1;
    //     }

    //     // TODO could use atoi instead to get the value
    //     if (ch != '0') {
    //         *value = 1;
    //     } else {
    //         *value = 0;
    //     }

    //     DEBUG_PRINT("GPIO fd %d value %c read\n",fd,ch);

    //     return 0;
    // }



    /****************************************************************
     * gpio_fd_open
     ****************************************************************/

    int pwm_duty_cycle_fd_open(unsigned int pwm)
    {
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_PWM_DIR "/pwm%d/duty_cycle", pwm);
        FW_ASSERT(len > 0, len);

        fd = open(buf, O_RDWR | O_NONBLOCK );
        if (fd < 0) {
            DEBUG_PRINT("pwm/duty_cycle_fd_open error!\n");
            return -1;
        }
        return fd;
    }

    /****************************************************************
     * gpio_fd_close
     ****************************************************************/

    int pwm_fd_close(int fd, unsigned int pwm)
    {
        // TODO is this needed? w/o this the edge file and others can retain the state from
        // previous settings.
        (void) pwm_unexport(pwm); // TODO check return value

        return close(fd);
    }

    bool LinuxPWMDriverComponentImpl ::
    open(NATIVE_INT_TYPE pwm, PWMPolarity polarity) {

      // TODO check for invalid gpio?
      NATIVE_INT_TYPE stat;

      // Configure:
      stat = pwm_export(pwm);
      if (-1 == stat) {
          Fw::LogStringArg arg = strerror(errno);
          this->log_WARNING_HI_PWM_OpenError(pwm,stat,arg);
          return false;
      }
      //TODO: way to determine if polarity can be changed?
    //   this->polarity = polarity;
    //   stat = pwm_set_polarity(pwm, this->polarity);
    //   if (-1 == stat) {
    //       Fw::LogStringArg arg = strerror(errno);
    //       this->log_WARNING_HI_PWM_OpenError(pwm,stat,arg);
    //       return false;
    //   }
      stat = pwm_set_period(pwm, period);
      if (-1 == stat) {
          Fw::LogStringArg arg = strerror(errno);
          this->log_WARNING_HI_PWM_OpenError(pwm,stat,arg);
          return false;
      }
      stat = pwm_set_enable(pwm, 1);
      if (-1 == stat) {
          Fw::LogStringArg arg = strerror(errno);
          this->log_WARNING_HI_PWM_OpenError(pwm,stat,arg);
          return false;
      }

      // If needed, set edge to rising in intTaskEntry()

      // Open:
      this->m_fd = pwm_duty_cycle_fd_open(pwm);
      if (-1 == this->m_fd) {
          Fw::LogStringArg arg = strerror(errno);
          this->log_WARNING_HI_PWM_OpenError(pwm,errno,arg);
      } else {
            this->m_pwm = pwm;
            stat = pwm_set_duty_cycle(this->m_fd, 0);
            if (-1 == stat) {
                Fw::LogStringArg arg = strerror(errno);
                this->log_WARNING_HI_PWM_OpenError(pwm,stat,arg);
                return false;
            }
      }

      return true;
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void LinuxPWMDriverComponentImpl ::
    dutyCycleWrite_handler(
        const NATIVE_INT_TYPE portNum,
        F32 duty_cycle
    )
  {
    // TODO: check input and cap setting
    F32 dc = duty_cycle * period;
    pwm_set_duty_cycle(m_fd, static_cast<U32>(dc));
  }

} // end namespace Tank
