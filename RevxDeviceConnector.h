#pragma once
#include "revxobject.h"

#include "qstring.h"

#include <memory>

class QSettings;

class RevxDeviceConnector :
	public RevxObject
{
public:
	
	RevxDeviceConnector();
	~RevxDeviceConnector(void);

	bool connect();
	void disconnect();

private:

	bool tryToConnect();

	//actions of tryToConnect()
	virtual bool openCameras();
	virtual bool activateProbe();
	virtual bool loadAdvancedParameters();

	virtual void disconnectAll();

	std::unique_ptr<QSettings> _settings;
	QString _error;

};

