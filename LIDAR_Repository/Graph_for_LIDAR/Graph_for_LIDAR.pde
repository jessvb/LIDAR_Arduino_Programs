// Graphing sketch

// This program takes ASCII-encoded strings
// from the serial port at 9600 baud and graphs them. It expects values followed
// by a newline, or newline and carriage return

import processing.serial.*;          // For serial (Arduino) communication
import org.gicentre.utils.stat.*;    // For chart classes.
import java.util.List;               // For List objects.

Serial myPort;                  // The serial port
XYChart lineChart;              // An x-y graph from the GiCentre Library
ArrayList<PVector> velocities;  // List of x & y values (time and velocity)
float initialTime = System.nanoTime()/1E9; // Time when program starts
final int POINTS_ON_SCREEN = 150;  // The maximum number of data points on screen

void setup () {
  //---------- CHART SETUP ----------//
  // set the window size:
  size(400, 300);

  // set the font for the graph:
  textFont(createFont("Arial", 10), 10);

  // create a new x-y chart object:
  lineChart = new XYChart(this);

  // axis formatting and labels:
  lineChart.showXAxis(true);
  lineChart.showYAxis(true);

  lineChart.setYFormat("###.#### m/s");  // Velocity in m/s
  lineChart.setXFormat("###.#### s");      // Time in seconds

  lineChart.setMaxY(3);
  lineChart.setMinY(-3);

  // symbol colours:
  lineChart.setPointColour(color(180, 50, 50, 100));
  lineChart.setPointSize(5);
  lineChart.setLineWidth(2);
  background(255); // set background colour to white

  velocities = new ArrayList<PVector>();


  //---------- SERIAL SETUP ----------//
  // List all the available serial ports to debug which COM the Arduino is on:
  println(Serial.list()); // The first listed COM will be in [0], second COM in [1] etc.

  // Open the serial array bucket that the Arduino is on (for ex: COM9 might be [1]):
  myPort = new Serial(this, Serial.list()[1], 9600);

  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
}

void draw () {
  // everything happens in the serialEvent()
}

void serialEvent (Serial myPort) {
  //---------- GET DATA FROM ARDUINO ----------//
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');

  if (inString != null) {
    // trim off any whitespace:
    inString = trim(inString);
    // convert to an int:
    float inByte = float(inString);

    // add new value to velocity list:
    velocities.add(new PVector(System.nanoTime()/1E9 - initialTime, inByte));
    if (velocities.size() > POINTS_ON_SCREEN) { // Remove values from list if more than max
      velocities.remove(0);
    }

    //---------- DRAW GRAPH ----------//
    background(255); // set background colour to white
    textSize(10); // For the axis lables
    lineChart.setData(velocities); // set the chart data
    lineChart.draw(15, 15, width-30, height-30);

    // draw title over the graph (after graph has been drawn):
    fill(120);
    textSize(20);
    text("LIDAR Velocity Readings", 70, 30);

    try {
      Thread.sleep(100); // sleep 100 milliseconds
    } 
    catch(InterruptedException ex) {
    }
  }
}

