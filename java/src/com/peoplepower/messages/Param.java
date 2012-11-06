package com.peoplepower.messages;

public class Param {

  private String myName;
  
  private String myValue;
  
  private int myIndex = -1;
  
  /**
   * Empty constructor
   */
  public Param() {
  }
  
  /**
   * Constructor
   * @param name
   * @param value
   */
  public Param(String name, String value) {
    myName = name;
    myValue = value;
  }
  
  /**
   * Constructor
   * @param name
   * @param value
   */
  public Param(String name, String value, int index) {
    myName = name;
    myValue = value;
    myIndex = index;
  }

  /**
   * 
   * @param param
   * @return true if the two parameters have the same name and index
   */
  public boolean matches(Param param) {
    return param.getName().equals(myName) && param.getIndex() == myIndex;
  }

  public void setName(String name) {
    myName = name;
  }
  
  public void setValue(String value) {
    myValue = value;
  }
  
  public void setIndex(String index) {
    myIndex = Integer.parseInt(index);
  }
  
  public void setIndex(int index) {
    myIndex = index;
  }
  
  public String getName() {
    return myName;
  }

  public String getValue() {
    return myValue;
  }
  
  public int getIndex() {
    return myIndex;
  }

  public boolean hasIndex() {
    return myIndex > -1;
  }
}
