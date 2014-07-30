/**
 * 
 */
package com.peoplepowerco;

import java.util.Properties;

import org.python.util.PythonInterpreter; 

/**
 * This class serves as an example of how to run python scripts from Java
 * and perform basic functions like pass in arguments to the python script
 * @author David Moss
 *
 */
public class RunPython {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		String argv[] = { "Hello", "from", "python" };
		
		// Avoid passing in too much information into untrusted scripts
		// Erase System Properties
		Properties props = new Properties();
		props.setProperty("python.path", "/Users/ppc/workspace/ioeplatform/uxtract/java/");
		PythonInterpreter.initialize(null, props, argv);
		
		// Execute the python script
		try {
			PythonInterpreter py = new PythonInterpreter();
			py.execfile("pythonTest.py");
			
		} catch (org.python.core.PyException e) {
			throw e;
		}

	}

}
