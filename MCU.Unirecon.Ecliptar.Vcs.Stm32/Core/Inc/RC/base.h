/*
 * ecliptar_rc_ibus.h
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#ifndef RC_BASE_H
#define RC_BASE_H

#include <stdio.h>
#include <inttypes.h>

class ControlChannel
{
  public:
  int 							  	index;
  volatile float          	  		controlValue;
  uint16_t      	    	      	minPulseWidth;
  uint16_t        	          		maxPulseWidth;
  uint16_t               	  		midPulseWidth;
  int                     		  	signalLostPosition;
  volatile uint16_t       		innerPulseWidth;
  volatile uint16_t       		pulseWidth;
  volatile int16_t         		pulseDelta;
  volatile uint8_t         		available :1;
};

class RemoteController // @suppress("Class has a virtual method and non-virtual destructor")
{
public:

	virtual bool					Attached() = 0;

	virtual ControlChannel*   	ReadChannels() = 0;

	virtual bool            		Available() = 0;

	virtual void           		PrintInfoReport(ControlChannel* channels) = 0;

};


#endif /* RC_IBUS_H */
