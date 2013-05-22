package com.peoplepower.messages;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

public class Measurement {

  /** The List of params composing this measurement */
  private List<Param> myParams;
  
  /** The timestamp this measurement was created */
  private String myTimestamp;
  
  /** The device that generated this measurement */
  private String myDeviceId;
  
  /** The device type will let us <add> this device to the server */
  private int myDeviceType = -1;
  
  /**
   * Constructor
   */
  public Measurement(String deviceId) {
    setTimestamp();
    myDeviceId = deviceId;
    myParams = new ArrayList<Param>();
  }
  
  /**
   * Constructor
   */
  public Measurement(String deviceId, Param param) {
    setTimestamp();
    myDeviceId = deviceId;
    myParams = new ArrayList<Param>();
    myParams.add(param);
  }
  
  /**
   * Constructor, lets us also <add> this device to the user's account
   * through the proxy that is already registered to the user's account
   */
  public Measurement(String deviceId, int deviceType) {
    setTimestamp();
    myDeviceId = deviceId;
    myDeviceType = deviceType;
    myParams = new ArrayList<Param>();
  }
  
  public void addParam(Param param) {
    myParams.add(param);
  }
  
  public Iterator<Param> iterator() {
    return myParams.iterator();
  }
  
  public String getTimestamp() {
    return myTimestamp;
  }
  
  public String getDeviceId() {
    return myDeviceId;
  }
  
  public int getDeviceType() {
    return myDeviceType;
  }
  
  public boolean hasDeviceType() {
    return myDeviceType > -1;
  }
  
  
  /**
   * Set the local timestamp
   */
  private void setTimestamp() {
    myTimestamp = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss").format(new Date());
    
    // Timezone needs to be in the form -HH:mm, so I do that manually:
    String rawTimezone = new SimpleDateFormat("Z").format(new Date());
    myTimestamp = myTimestamp + rawTimezone.substring(0,3) + ":" + rawTimezone.substring(3);
  }
  
}
