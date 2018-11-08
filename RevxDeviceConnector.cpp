#include "RevxDeviceConnector.h"

#include "RevxMessage.h"

#include "FP7.h"
#include "Private.h"

#include "qsettings.h"
#include <iostream>

//added

RevxDeviceConnector::RevxDeviceConnector()
{
	_settings= std::unique_ptr<QSettings>(new QSettings);
}


RevxDeviceConnector::~RevxDeviceConnector(void)
{

}

void RevxDeviceConnector::disconnect()
{
	disconnectAll();
}

void RevxDeviceConnector::disconnectAll()
{
	FP7close();
}

bool RevxDeviceConnector::connect()
{
	
	if(tryToConnect()==false){
		disconnectAll();
		RevxMessage::showError(_error);
		return false;
	}

	return true;

}

bool RevxDeviceConnector::tryToConnect()
{
	
	if(openCameras()==false)
		return false;

	if(activateProbe()==false)
		return false;

	if(loadAdvancedParameters()==false)
		return false;

	return true;
}

bool RevxDeviceConnector::openCameras()
{
	//read setting of inrevxDirectory path
	
	QString revxDirectory = _settings->value("revxDirectory").toString();

	if(revxDirectory.isEmpty()) {
		_error="RevXperts Directory is empty!";
		return false;
	}

	QByteArray qRevxDirectory = revxDirectory.toLocal8Bit();
    char* cstyleRevxDirectory = qRevxDirectory.data();

	//read setting of cameraMode

	QString cameraMode=_settings->value("cameraMode").toString();

	if(cameraMode.isEmpty()) {
		_error="Setting item - cameraMode is empty!";
		return false;
	}

    //read setting of camera serial numbers 

	char *sensorSN[2];
	int cameraNumber=0;

	QByteArray qSingleSerialNumber;
	QByteArray qMasterSerialNumber;
	QByteArray qSlaveSerialNumber;

	if(cameraMode=="single") {

		cameraNumber=1;
		
		QString singleSerialNumber=_settings->value("singleSerialNumber").toString();
		if(singleSerialNumber.isEmpty()) {
			_error="Setting item - singleSerialNumber is empty!";
			return false;
		}
		qSingleSerialNumber = singleSerialNumber.toLocal8Bit();
        sensorSN[0] = qSingleSerialNumber.data();
		
	} else {

		cameraNumber=2;
		
		QString masterSerialNumber=_settings->value("masterSerialNumber").toString();
		if(masterSerialNumber.isEmpty()) {
			_error="Setting item - masterSerialNumber is empty!";
			return false;
		}
		qMasterSerialNumber = masterSerialNumber.toLocal8Bit();
        sensorSN[0] = qMasterSerialNumber.data();

		QString slaveSerialNumber=_settings->value("slaveSerialNumber").toString();
		if(slaveSerialNumber.isEmpty()) {
			_error="Setting item - slaveSerialNumber is empty!";
			return false;
		}
		qSlaveSerialNumber= slaveSerialNumber.toLocal8Bit();
        sensorSN[1] = qSlaveSerialNumber.data();

	}

	//open cameras

	int status=0;

	status=FP7openEx(cameraNumber, sensorSN, cstyleRevxDirectory, 1 );
	if( status != FP7_SUCCESS ) {
		_error="Camera cannot be opened!";
		return false;
	}

	status=FP7setButtons(2);
	if( status != FP7_SUCCESS ) {
		_error="Button Number cannot be sent to the camera!";
		return false;
	}

	//if here is reached, cameras are opened successfully 
	return true;
}

bool RevxDeviceConnector::activateProbe()
{
	//read setting of tool file name

	QString probeFileName=_settings->value("probeFileName").toString();
	if(probeFileName.isEmpty())
	{
		_error="Setting item - probeFileName is empty!";
		return false;
	}
	QByteArray qProbeFileName = probeFileName.toLocal8Bit();
    char *fileName = qProbeFileName.data();

	//read setting of probe port

	QString probePort=_settings->value("probePort").toString();
	if(probePort.isEmpty())
	{
		_error="Setting item - probePort is empty!";
		return false;
	}
	QByteArray qProbePort = probePort.toLocal8Bit();
    char *cstyleProbePort = qProbePort.data();

	//open the probe

	int status=0;
	status = FP7activateProbe( *cstyleProbePort, fileName, 1 );
	if(status != FP7_SUCCESS)
	{
		_error="Probe cannot be activated!";
		return false;
	}

	//read setting of tipLength and send to the device

	QString tipLength=_settings->value("tipLength").toString();
	if(tipLength.isEmpty())
	{
		_error="Setting item - tipLength is empty!";
		return false;
	}
	double length=tipLength.toDouble();

	status = FP7setTipLength( *cstyleProbePort, length ); 
	if(status != FP7_SUCCESS)
	{
		_error="tipLength cannot be sent to the camera!";
		return false;
	}

	//if here is reached, the probe is activated successfully 
	return true;
}

bool RevxDeviceConnector::loadAdvancedParameters()
{
	//read setting of units and send to the device

	QString unit=_settings->value("unit").toString();
	if(unit.isEmpty())
	{
		_error="Setting item - unit is empty!";
		return false;
	}
	
	int status=0;

	if (unit=="millimeter")
		status = FP7setUnits(UNITS_MM);
	else
		status = FP7setUnits(UNITS_IN);

	if(status != FP7_SUCCESS)
	{
		_error="unit cannot be sent to the camera!";
		return false;
	}

	//read setting of geometryTolerance and send to the device

	QString geometryTolerance=_settings->value("geometryTolerance").toString();
	if(geometryTolerance.isEmpty())
	{
		_error="Setting item - geometryTolerance is empty!";
		return false;
	}
	double tolerance=geometryTolerance.toDouble();

	status = FP7setTolerance(tolerance);
	if(status != FP7_SUCCESS)
	{
		_error="geometryTolerance cannot be sent to the camera!";
		return false;
	}

	//read setting of exposureTimes and send to the device

	QString exposureTimes=_settings->value("exposureTimes").toString();
	if(exposureTimes.isEmpty())
	{
		_error="Setting item - exposureTimes is empty!";
		return false;
	}
	double times=exposureTimes.toDouble();

	status = FP7setExposure(times);
	if(status != FP7_SUCCESS)
	{
		_error="exposureTimes cannot be sent to the camera!";
		return false;
	}

	//read setting of exposureExtension and send to the device

	QString exposureExtension=_settings->value("exposureExtension").toString();
	if(exposureExtension.isEmpty())
	{
		_error="Setting item - exposureExtension is empty!";
		return false;
	}
	double extension=exposureExtension.toDouble();

	status = FP7setExtraIntegration(extension);
	if(status != FP7_SUCCESS)
	{
		_error="exposureExtension cannot be sent to the camera!";
		return false;
	}
	
	//if here is reached, advanced parameters are loaded successfully 
	return true;
}