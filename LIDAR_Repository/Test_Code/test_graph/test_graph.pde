import org.gicentre.utils.stat.*;    // For chart classes.
import java.util.List;               // For List object.

// Displays a simple line chart representing a time series.

XYChart lineChart;
ArrayList<PVector> velocities;
float firstTime = System.nanoTime()/1E9;

// Loads data into the chart and customises its appearance.
void setup()
{
  size(400, 300);
  textFont(createFont("Arial", 10), 10);

  // Both x and y data set here.  
  lineChart = new XYChart(this);

  velocities = new ArrayList<PVector>(100);
  velocities.add(new PVector(1.0, 1.01));
  velocities.add(new PVector(2.0, 1.015));
  velocities.add(new PVector(3.0, 1.02));
  velocities.add(new PVector(4.0, 1.027));
  velocities.add(new PVector(5, 1.023));
  velocities.add(new PVector(6, 1.016));
  velocities.remove(0);

  lineChart.setData(velocities);

  // Axis formatting and labels.
  lineChart.showXAxis(true); 
  lineChart.showYAxis(true); 
  //  lineChart.setMinY(0);

  lineChart.setYFormat("###.#### m/s");  // Monetary value in $US
  lineChart.setXFormat("###.#### s");      // Time in sec

  // Symbol colours
  lineChart.setPointColour(color(180, 50, 50, 100));
  lineChart.setPointSize(5);
  lineChart.setLineWidth(2);
}

// Draws the chart and a title.
void draw()
{
  background(255); // White BG
  textSize(10); // For the axis lables
  lineChart.draw(15, 15, width-30, height-30); // Draw the chart

  // Draw a title over the top of the chart.
  fill(120);
  textSize(20);
  text("LIDAR Velocity Readings", 70, 30);

  try {
    Thread.sleep(100);                 // sleep 100 milliseconds
  } 
  catch(InterruptedException ex) {
    println(ex);
  }

  
  velocities.add(new PVector(System.nanoTime()/1E9 - firstTime, 10));
  if (velocities.size() > 50)
    velocities.remove(0);
  lineChart.setData(velocities);
}

