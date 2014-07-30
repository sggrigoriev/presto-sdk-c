/**
 * 
 */
package com.peoplepowerco;

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
		// Erase Properties
		PythonInterpreter.initialize(null, null, argv);
		
		// Execute the python script
		PythonInterpreter py = new PythonInterpreter();
		py.execfile("pythonTest.py");
		
	}

}
