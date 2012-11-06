package com.peoplepower.messages;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import com.peoplepower.messages.Measurement;
import com.peoplepower.messages.Param;

/**
 * A Device has a unique ID, and contains a list of 
 * parameters to manage. We can extract a time stamped Measurement
 * from a device at any time.
 * 
 * @author David Moss
 *
 */
public class Device {

  /** The List of params composing this measurement */
  private List<Param> myParams;
  
  /** The device type of this device */
  private int myDeviceType;
  
  /** The device that generated this measurement */
  private String myDeviceId;
  
  /**
   * Constructor
   * @param deviceId the globally unique ID of this device
   * @param deviceType the device type assigned by the People Power cloud server
   */
  public Device(String deviceId, int deviceType) {
    myDeviceId = deviceId;
    myDeviceType = deviceType;
    myParams = new ArrayList<Param>();
  }
  
  /**
   * Update a parameter on this device, or create a new
   * parameter if the given parameter is not being tracked.
   * Parameters are tracked by name and index.
   * 
   * @param param Parameter to update on this device
   */
  public void updateParam(Param param) {
    Param focusedParam;
    
    for(Iterator<Param> it = myParams.iterator(); it.hasNext(); ) {
      focusedParam = (Param) it.next();
      if(focusedParam.matches(param)) {
        focusedParam.setValue(param.getValue());
        return;
      }
    }
    
    // A match was not found, create this parameter
    focusedParam = new Param(param.getName(), param.getValue(), param.getIndex());
    myParams.add(focusedParam);
  }
  
  public Iterator<Param> iterator() {
    return myParams.iterator();
  }
  
  public String getDeviceId() {
    return myDeviceId;
  }
  
  public int getDeviceType() {
    return myDeviceType;
  }
  
  /**
   * Extract a measurement from this device
   * @return a new time stamped Measurement
   */
  public Measurement extractMeasurement() {
    Measurement m = new Measurement(myDeviceId, myDeviceType);
    Param focusedParam;
    
    for(Iterator<Param> it = myParams.iterator(); it.hasNext(); ) {
      focusedParam = (Param) it.next();
      m.addParam(focusedParam);
    }
    
    return m;
  }
}
