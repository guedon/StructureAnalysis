"""Sequences

.. author:: Thomas Cokelaer, Thomas.Cokelaer@inria.fr
uthor:/

"""
__revision__ = "$Id: $"

import os
import openalea.stat_tool.interface as interface
from openalea.sequence_analysis._sequence_analysis import \
    _Sequences, \
    _Renewal_data,\
    _Markovian_sequences,\
    _Variable_order_markov_data,\
    _Semi_markov_data,\
    _Nonhomogeneous_markov_data
    
    
#import _sequence_analysis
from openalea.stat_tool import error



from enumerate import index_parameter_type_map, ms_vomd_smd_nhmd
from openalea.stat_tool.enumerate import bool_type

__all__ = ['Sequences',
           '_Sequences',
           'LumpabilityTest',
           'RemoveIndexParameter',
           'TransformPosition', 
           'SaveMTG', 
           'ComputeInitialRun',
           'Split']


# Extend dynamically class
interface.extend_class( _Sequences, interface.StatInterface)

# Add methods to _Vectors


def Sequences(obj, **kargs):
    """Construction of a set of sequences from multidimensional arrays
    of integers, from data generated by a renewal process or from an
    ASCII file.

    The data structure of type array(array(array(int))) should be
    constituted at the most internal level of arrays of constant size. If the
    optional argument IndexParameter is set at "Position" or "Time", the data
    structure of type array(array(array(int))) is constituted at the most
    internal level of arrays of size 1 + n (index parameter, n variables
    attached to the explicit index parameter). If the optional argument
    IndexParameter is set at "Position", only the index parameter of the
    last array of size 1 + n is considered and the first component of successive
    elementary arrays (representing the index parameter) should be
    ncreasing. If the optional argument IndexParameter is set at "Time", the
    first component of successive elementary arrays should be strictly
    increasing.

  
    :Parameters:

    * array1 (array(array(int))): input data for univariate sequences
    * arrayn (array(array(array(int)))): input data for multivariate sequences,
    * timev (renewal_data),
    * file_name (string).
    
    :Optional Parameters:
   
    * Identifiers (array(int)): explicit identifiers of sequences. This optional argument can only be used if the first argument is of type array(array(int / array(int))).
    * IndexParameter (string): type of the explicit index parameter: "Position" or "Time" (the default: implicit discrete index parameter starting at 0). This optional argument can only be used if the first argument is of type array(array(int / array(int))).

    :Returns:
    
    If the construction succeeds, an object of type sequences or discrete_sequences is returned, otherwise no object is returned. The returned object is of type discrete_sequences if all the variables are of type STATE, if the possible values for each variable are consecutive from 0 and if the number of possible values for each variable is <= 15.

    :Examples:
      
    >>> Sequences(array1, Identifiers=[1, 8, 12])
    >>> Sequences(arrayn, Identifiers=[1, 8, 12], IndexParameter="Position")
    >>> Sequences(timev)
    >>> Sequences(file_name)    
    
    .. seealso::    
    
       :class:`~openalea.stat_tool.output.Save`,
       :func:`~openalea.sequence_analysis.data_transform.AddAbsorbingRun`,
       :func:`~openalea.stat_tool.cluster.Cluster`,
       :func:`~openalea.sequence_analysis.data_transform.Cumulate`,
       :func:`~openalea.sequence_analysis.data_transform.Difference`,
       :func:`~openalea.sequence_analysis.data_transform.IndexParameterExtract`, 
       :func:`~openalea.sequence_analysis.data_transform.LengthSelect`,
       :func:`~openalea.stat_tool.data_transform.Merge`,
       :func:`~openalea.stat_tool.data_transform.MergeVariable`,
       :func:`~openalea.sequence_analysis.data_transform.MovingAverage`,
       :func:`~openalea.sequence_analysis.data_transform.RecurrenceTimeSequences`,
       :func:`~openalea.sequence_analysis.data_transform.RemoveRun`,
       :func:`~openalea.sequence_analysis.data_transform.Reverse`,
       :func:`~openalea.sequence_analysis.data_transform.SegmentationExtract`,
       :func:`~openalea.stat_tool.data_transform.SelectIndividual`, 
       :func:`~openalea.stat_tool.data_transform.SelectVariable`,
       :func:`~openalea.stat_tool.data_transform.Shift`,
       :func:`~openalea.stat_tool.cluster.Transcode`, 
       :func:`~openalea.stat_tool.data_transform.ValueSelect`,
       :func:`~openalea.sequence_analysis.data_transform.VariableScaling`.
       :func:`~openalea.stat_tool.data_transform.ExtractHistogram`, 
       :func:`~openalea.sequence_analysis.data_transform.ExtractVectors`, 
       :func:`~openalea.sequence_analysis.correlation.ComputeCorrelation`, 
       :func:`~openalea.sequence_analysis.correlation.ComputePartialAutoCorrelation`, 
       :func:`~openalea.sequence_analysis.data_transform.ComputeSelfTransition`, 
       :func:`~openalea.sequence_analysis.compare.Compare`, 
       :func:`~openalea.sequence_analysis.estimate.Estimate`, 
       :func:`~openalea.sequence_analysis.data_transform.ComputeStateSequences`, 
       :func:`~openalea.sequence_analysis.simulate.Simulate`.

        
    .. todo:: refactoring using AML original code
    """ 
    from openalea.sequence_analysis._sequence_analysis import TIME, POSITION, \
        IMPLICIT_TYPE
    #error.CheckArgumentsLength(args, 1, 1)
    
    
    index_parameter = error.ParseKargs(kargs, "IndexParameter", 
                                       "IMPLICIT_TYPE",
                                       index_parameter_type_map)
    
    Identifiers = error.ParseKargs(kargs, "Identifiers", None)
    
    # all values must be positive strictly
    if Identifiers:
        assert len([x for x in Identifiers if x<=0]) == 0
        #todo: assert type([x for x in Identifiers])==int
        
    sequence = None

    error.CheckType([obj], [[str, _Renewal_data, list]])
    
    if isinstance(obj, str):
        filename = obj
        if os.path.isfile(filename):
            OldFormat = error.ParseKargs(kargs, "OldFormat", False, bool_type)
            sequence = _Sequences(filename, OldFormat)
        else:
            raise IOError("bad file name")
        
    elif isinstance(obj, _Renewal_data):
        sequence = _Sequences(obj)
    # otherwise, we switch to a list constructor that requires a list of seqs
    # and a list of identifiers. The latter being optional        
    else: # must be a list
        
        if kargs.get("IndexParameter") and obj.nb_variable == 1 : 
            raise ValueError("""
            IndexParameter cannot be used if nb_variable equals 1""")
          
        
        valid_param = [POSITION, TIME, IMPLICIT_TYPE]
        if index_parameter not in valid_param:
            raise ValueError("""IndexParameter can be only %s if first 
            argument is a list""" % valid_param)
        if Identifiers is None:  
            sequence = _Sequences(obj, range(0, len(obj)),
                              index_parameter)        
        else:
        
            sequence = _Sequences(obj, Identifiers, index_parameter)
    
    if hasattr(sequence, 'markovian_sequences'):
        try:
            sequence = sequence.markovian_sequences()
        except Exception:
            pass
   
    return sequence
    
    
def LumpabilityTest(obj, *args, **kargs):    
    """.. todo:: documenation"""
    
    error.CheckArgumentsLength(args, 1, 1)
    error.CheckType([obj], [[_Markovian_sequences, _Variable_order_markov_data,
                            _Semi_markov_data, _Nonhomogeneous_markov_data]])
    
    
    symbol = args[0]
    Order = kargs.get("Order", 1)
    
    error.CheckType([symbol, Order], [list, int])
    
    ret = obj.lumpability_test(symbol, Order)
        
    if ret is False:
        raise TypeError("warning: false status returned by lumpability test")
        
  
def RemoveIndexParameter(obj):
    """.. todo:: documenation
    
    input can be sequence, markovian_sequences, 
    nonhomogeneous_markov, variable_order_markov"""
    
    error.CheckType([obj], [[_Sequences, _Markovian_sequences,
                             _Variable_order_markov_data, _Semi_markov_data, 
                             _Nonhomogeneous_markov_data]])
    if isinstance(obj, _Sequences):
        return obj.remove_index_parameter().markovian_sequences()
    else:
        return obj.remove_index_parameter()

def TransformPosition(obj, step=None):
    """.. todo:: documenation
    
    input is a sequence only"""
    error.CheckType([obj, step], [_Sequences, int])
    
    ret = obj.transform_position(step)
    if hasattr(obj, 'markovian_sequences'):
        return obj.markovian_sequences()
    else:
        return ret

def ComputeInitialRun(obj):
    """.. todo:: documenation
    
    input can be sequence, markovian_sequences, nonhomogeneous_markov, 
    variable_order_markov
    """
    error.CheckType([obj], [[_Markovian_sequences, _Variable_order_markov_data,
                             _Semi_markov_data, _Nonhomogeneous_markov_data]])
    return obj.initial_run_computation()


def Split(obj, step):
    """.. todo:: documentaiton
    
    input markovian
    """
    error.CheckType([obj], [[_Markovian_sequences, _Variable_order_markov_data, 
                            _Semi_markov_data, _Nonhomogeneous_markov_data]])
    error.CheckType([step], [int])
    
    return obj.split(step)
        
def SaveMTG(obj, Filename=None, Type=None):
    """
    Save sequence in MTG format.
    :param Type : list of "S" or "N" characters for Symbolic or Numeric
    """
    from openalea.stat_tool.enumerate import sub_variable_type
    
    error.CheckType([obj], [ms_vomd_smd_nhmd])
    error.CheckType([Filename, Type], [str, list])
    
    Type = []
    for pstr in Type:
        Type.append(sub_variable_type[pstr])
    
    obj.mtg_write(Filename, Type)
