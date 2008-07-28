#ifndef __PLOTABLE__
#define __PLOTABLE__

#include <list>
#include <vector>
#include <utility>
#include <iterator>
#include <string>

using namespace std;

namespace plotable
{

  typedef std::pair<float, float> PlotPoint;
  typedef std::pair<float, float> Range;
  
  /* Class SinglePlot */
  /* list of (X,Y) pair with properties */
  
  class SinglePlot
  {
    
    typedef std::list< PlotPoint > Points;

  public:
    Points data;
    string legend;
    string style;

    /* Combination with
    /* impulse or linepoints*/
    /* linestyle : -- : -. - */
    /* marker : + , o . s v x > < ^ */

    string color; 

    /*   b  : blue */
    /*   g  : green */
    /*   r  : red */
    /*   c  : cyan */
    /*   m  : magenta */
    /*   y  : yellow */
    /*   k  : black */
    /*   w  : white */
    
    
  public:
    SinglePlot() {};

    void add_point(float x, float y);
    void add_point(const PlotPoint&  p);
    
    int size()
    { return data.size(); };   

    Points::const_iterator begin()
      { return data.begin(); };

    Points::const_iterator end()
      { return data.end(); };
   
  };
  
  
  
  /* Class MultiPlot */
  /* Set of SinglePlot */
  class MultiPlot
  {
    std::vector<SinglePlot> plots;
    
  public:
    string title;

    float xtics; 
    float ytics;

    Range xrange;
    Range yrange;

    string xlabel;
    string ylabel;

    bool grid;

    MultiPlot(int size=1)
      :plots(size), xtics(0), ytics(0), 
      xrange(0,0), yrange(0,0), grid(false)
      {};
    
    SinglePlot& operator[](int index)
      { return plots[index]; };
    
    void resize(int newsize)
    { plots.resize(newsize); };

    int size()
    { return plots.size(); };

    std::vector<SinglePlot>::const_iterator begin()
      { return plots.begin(); };

    std::vector<SinglePlot>::const_iterator end()
      { return plots.end(); };


    
  };
    

  /* Class MultiPlotSet */
  /* List of MultiPlot */
  class MultiPlotSet
  {
    std::vector<MultiPlot> multiplots;

  public:

    string title;
    string border;
    
    MultiPlotSet(int size)
      :multiplots(size)
      {};
    
    MultiPlot& operator[](int index)
      { return multiplots[index]; };

    int size()
    { return multiplots.size(); };

    std::vector<MultiPlot>::const_iterator begin()
      { return multiplots.begin(); };

    std::vector<MultiPlot>::const_iterator end()
      { return multiplots.end(); };

  };
    

      

}; //namespace plotable

#endif
