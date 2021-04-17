#include "rc.h"

	void 				RemoteControl::Attach(RemoteController* rc)
	{
		this->rc = rc;
	}

	bool            	RemoteControl::Attached()
	{
		return rc->Attached();
	}

	bool            	RemoteControl::Available()
	{
		return rc->Available();
	}

	ControlChannel*   	RemoteControl::ReadChannels()
	{
		return rc->ReadChannels();
	}

	void           		RemoteControl::PrintInfoReport(ControlChannel* channels)
	{
		rc->PrintInfoReport(channels);
	}
