"""to be done"""
__revision__ = "$Id$"

import _stat_tool 
import openalea.stat_tool as stat_tool

stat_tool.Parametric = _stat_tool._ParametricModel
stat_tool.Histogram = _stat_tool._DistributionData

stat_tool.Parametric.simulation = _stat_tool._ParametricModel.simulate
_stat_tool._DistributionData.Plot = _stat_tool._DistributionData.plot
_stat_tool._DistributionData.Display = _stat_tool._DistributionData.display
_stat_tool._ParametricModel.Plot = _stat_tool._ParametricModel.plot
_stat_tool._ParametricModel.Display = _stat_tool._ParametricModel.display