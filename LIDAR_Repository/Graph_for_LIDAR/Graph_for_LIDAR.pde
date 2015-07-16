// Graphing sketch

// This program takes ASCII-encoded strings
// from the serial port at 9600 baud and graphs them. It expects values followed
// by a newline, or newline and carriage return

import processing.serial.*;          // For serial (Arduino) communication
import org.gicentre.utils.stat.*;    // For chart classes.
import java.util.List;               // For List objects.

Serial myPort;                  // The serial port
XYChart velChart;              // An x-y graph for velocities
ArrayList<PVector> velocities;  // List of x & y values (time and velocity)
XYChart distChart;             // An x-y graph for distances
ArrayList<PVector> distances;  // List of x & y values (time and distance)
float initialTime = System.nanoTime()/1E9; // Time when program starts
final int POINTS_ON_SCREEN = 150;  // The maximum number of data points on screen

void setup () {
  //---------- CHART SETUP ----------//
  // set the window size:
  size(800, 600);

  // set the font for the graph labels:
  textFont(createFont("Arial", 10), 10);

  // create new x-y chart objects:
  velChart = new XYChart(this);
  distChart = new XYChart(this);

  // axis formatting and labels:
  velChart.showXAxis(true);
  velChart.showYAxis(true);
  velChart.setYFormat("###.#### m/s");  // Velocity in m/s
  velChart.setXFormat("###.#### s");      // Time in seconds
  velChart.setMaxY(3);
  velChart.setMinY(-3);

  distChart.showXAxis(false);
  distChart.showYAxis(true);
  distChart.setYFormat("###.#### m");  // Distance in m
  distChart.setXFormat("###.#### s");      // Time in seconds
  distChart.setYAxisAt(velChart.getMaxX()); // Move the distance axis to the right side
  distChart.setMinY(0);

  // symbol colours:
  velChart.setPointColour(color(180, 50, 50, 100));
  velChart.setPointSize(5);
  velChart.setLineWidth(2);

  distChart.setPointColour(color(180, 50, 50, 100));
  distChart.setPointSize(4);
  distChart.setLineWidth(1);

  background(255); // set background colour to white

  velocities = new ArrayList<PVector>();
  distances = new ArrayList<PVector>();


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
  // get the ASCII string that contains a velocity and distance:
  String str = myPort.readStringUntil('\n'); // will be in this form: 1.1V0.59D

  if (str != null) {
    // trim off any whitespace:
    str = trim(str);
    // parse for the velocity and the distance:
    float velocity = float(str.substring(0, str.indexOf('V')));
    float distance = float(str.substring(str.indexOf('V')+1, str.length()-2));
    distance = distance/100; // distance in meters


    //---------- ADD NEW VELOCITY & DISTANCE TO LISTS ----------//
    float time = System.nanoTime()/1E9 - initialTime;
    velocities.add(new PVector(time, velocity));
    distances.add(new PVector(time, distance));
    if (velocities.size() > POINTS_ON_SCREEN) { // Remove values from list if more than max
      velocities.remove(0);
      distances.remove(0);
    }


    //---------- DRAW GRAPH ----------//
    background(255); // set background colour to white
    textSize(11); // For the axis lables
    velChart.setData(velocities); // set the chart data with the arrayList
    velChart.draw(15, 15, width-30, height-30);
    distChart.setData(distances); // set the chart data with the arrayList
    distChart.setYAxisAt(velChart.getMaxX()); // Move the distance axis to the right side
    distChart.draw(15, 15, width-30, height-30);

    // draw title over the graph (after graph has been drawn):
    fill(120);
    textSize(40);
    text("LIDAR Velocity & Distance Readings", 90, 50);
  }
}

