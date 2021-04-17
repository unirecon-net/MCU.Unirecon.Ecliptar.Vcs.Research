/*
 * ecliptar_rc_ibus.h
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#ifndef _RC_H
#define _RC_H

#include <stdio.h>
#include <inttypes.h>
#include <usart.h>

#include "RC/base.h"
#include "RC/ibus.h"

class RemoteControl : public RemoteController // @suppress("Class has a virtual method and non-virtual destructor")
{
public:
	void Attach(RemoteController* rc);

	virtual bool            	Attached();

	virtual bool            	Available();

	virtual ControlChannel*   	ReadChannels();

	virtual void           		PrintInfoReport(ControlChannel* channels);

	RemoteController* rc;

};


#endif /* _RC_H */
