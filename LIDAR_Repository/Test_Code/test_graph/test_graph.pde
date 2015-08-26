import org.gicentre.utils.stat.*;    // For chart classes.
import java.util.List;               // For List object.

// Displays a simple line chart representing a time series.

XYChart velChart;
XYChart distChart;
ArrayList<PVector> velocities;
ArrayList<PVector> distances;
double firstTime = System.nanoTime()/1E9;

// Loads data into the chart and customises its appearance.
void setup()
{
  size(1600, 600);
  textFont(createFont("Arial", 10), 10);

  // Both x and y data set here.  
  velChart = new XYChart(this);
  distChart = new XYChart(this);

  velocities = new ArrayList<PVector>(100);
  velocities.add(new PVector(1.0, 1.01));
  velocities.add(new PVector(2.0, 1.015));
  velocities.add(new PVector(3.0, 1.02));
  velocities.add(new PVector(4.0, 1.027));
  velocities.add(new PVector(5, 1.023));
  velocities.add(new PVector(6, 1.016));
  velocities.remove(0);

  distances = new ArrayList<PVector>(100);
  distances.add(new PVector(2.0, 5.05));
  distances.add(new PVector(3.0, 6.7));
  distances.add(new PVector(4.0, 3.2));
  distances.add(new PVector(5.0, 4.6));
  distances.add(new PVector(6, 5.5));

  velChart.setData(velocities);
  distChart.setData(distances);

  // Axis formatting and labels.
  velChart.showXAxis(true); 
  velChart.showYAxis(true); 
  velChart.setXAxisLabel("Time (s)");
  velChart.setYAxisLabel("Velocity (m/s)");
  velChart.setYFormat("###.#### m/s");  // Velocity in m/s
  velChart.setXFormat("###.#### s");      // Time in sec

  distChart.showXAxis(true); 
  distChart.showYAxis(true);
  distChart.setYAxisLabel("Distance (m)");
  distChart.setXAxisLabel("Time (s)");
  distChart.setYFormat("###.#### m");  // Distance in m
  distChart.setXFormat("###.#### s");      // Time in sec
  distChart.setMinY(0);

  // Symbol colours
  velChart.setPointColour(color(180, 50, 50, 50));
  velChart.setPointSize(5);
  velChart.setLineWidth(2);

  distChart.setPointColour(color(90, 50, 50, 100));
  distChart.setPointSize(5);
  distChart.setLineWidth(2);
}

// Draws the chart and a title.
void draw()
{
  background(255); // White BG
  textSize(11); // For the axis lables
  velChart.draw(15, 15, width-30-800, height-30); // Draw the chart
  fill(100, 111, 111);
  distChart.draw(15+800, 15, width-30-800, height-30); // Draw the chart
  fill(100, 111, 111);

  // Draw a title over the top of the chart.
  fill(120);
  textSize(20);
  text("LIDAR Velocity Readings", 90, 50);
  text("LIDAR Distance Readings", 90+800, 50);
  
  try {
    Thread.sleep(100);                 // sleep 100 milliseconds
  } 
  catch(InterruptedException ex) {
    println(ex);
  }

  velocities.add(new PVector((float)(System.nanoTime()/1E9 - firstTime), 10));
  distances.add(new PVector((float)(System.nanoTime()/1E9 - firstTime), 5));
  if (velocities.size() > 50) {
    velocities.remove(0);
    distances.remove(0);
  }
  velChart.setData(velocities);
  distChart.setData(distances);
}