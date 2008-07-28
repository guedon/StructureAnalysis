/*------------------------------------------------------------------------------
 *                                                                              
 *        VPlants.Stat_Tool : VPlants Statistics module
 *                                                                              
 *        Copyright 2006-2007 INRIA - CIRAD - INRA                      
 *                                                                              
 *        File author(s): Yann Guédon <yann.guedon@cirad.fr>
 *                        Jean-Baptiste Durand <Jean-Baptiste.Durand@imag.fr>
 *                        Samuel Dufour-Kowalski <samuel.dufour@sophia.inria.fr>
 *                        Christophe Pradal <christophe.prada@cirad.fr>         
 *                                                                              
 *        Distributed under the GPL 2.0 License.                               
 *        See accompanying file LICENSE.txt or copy at                          
 *           http://www.gnu.org/licenses/gpl-2.0.txt
 *                                                                              
 *        OpenAlea WebSite : http://openalea.gforge.inria.fr                    
 *       
 *        $Id$
 *                                                                       
 *-----------------------------------------------------------------------------*/

#include "export_plotable.h"
#include "stat_tool/plotable.h"

#include <boost/python.hpp>
#include <boost/python/detail/api_placeholder.hpp>
#include <boost/python/make_constructor.hpp>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;
using namespace plotable;
using namespace std;





void class_plotable()
{

  class_ < PlotPoint >("PlotPoint", init<float, float>())
    .def_readwrite("x", &PlotPoint::first )
    .def_readwrite("y", &PlotPoint::second )
    
    .def_readwrite("min", &PlotPoint::first )
    .def_readwrite("max", &PlotPoint::second );


//   class_ < Range >("Range", init<float, float>())
//     .def_readwrite("min", &PlotPoint::first )
//     .def_readwrite("max", &PlotPoint::second );


  // _Format_error
  class_< SinglePlot >("SinglePlot")
    
    .def("add_point", (void (SinglePlot::*)(const PlotPoint&)) &SinglePlot::add_point)
    .def("add_point", (void (SinglePlot::*)(float, float)) &SinglePlot::add_point)
    
    .def_readwrite("legend", &SinglePlot::legend)
    .def_readwrite("style", &SinglePlot::style)
    .def_readwrite("color", &SinglePlot::color)
    
    .def("__iter__", range(&SinglePlot::begin, &SinglePlot::end))
    .def("__len__", &SinglePlot::size)
    ;


  class_< MultiPlot >("MultiPlot", init<int>())

    .def_readwrite("title", &MultiPlot::title)
    .def_readwrite("xtics", &MultiPlot::xtics)
    .def_readwrite("ytics", &MultiPlot::ytics)

    .def_readwrite("xlabel", &MultiPlot::xlabel)
    .def_readwrite("ylabel", &MultiPlot::ylabel)

    .def_readwrite("xrange", &MultiPlot::xrange)
    .def_readwrite("yrange", &MultiPlot::yrange)

    .def_readwrite("grid", &MultiPlot::grid)

    .def("resize", &MultiPlot::resize)
    .def("__len__", &MultiPlot::size)
    .def("__getitem__", &MultiPlot::operator[],
	 return_internal_reference<>())
    .def("__iter__", range(&MultiPlot::begin, &MultiPlot::end))
    ;


  class_< MultiPlotSet >("MultiPlotSet", init<int>())
    .def_readwrite("title", &MultiPlotSet::title)
    .def_readwrite("border", &MultiPlotSet::border)

    .def("__len__", &MultiPlotSet::size)
    .def("__getitem__", &MultiPlotSet::operator[],
	 return_internal_reference<>())
    .def("__iter__", range(&MultiPlotSet::begin, &MultiPlotSet::end))
    ;


}



