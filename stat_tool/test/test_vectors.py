"""vectors tests"""
__revision__ = "$Id: $"

from openalea.stat_tool.vectors import Vectors, VectorDistance, \
    VarianceAnalysis, ContingencyTable
from openalea.stat_tool.data_transform import ValueSelect
from openalea.stat_tool.regression import Regression
from openalea.stat_tool.data_transform import ExtractHistogram, \
    SelectIndividual, SelectVariable
from openalea.stat_tool.comparison import Compare
from openalea.stat_tool.estimate import Estimate
from openalea.stat_tool.cluster import Clustering
from openalea.stat_tool.output import Display

class Test:

    def test_vectors_pylist(self):
        """test vector constructor from list"""
        v = Vectors([[0, 1, 2, 3], [4, 5, 6, 7]])
        assert v

        v = Vectors([[1.2, 0.34], [1.2, 0.34]])
        assert v

        v = Vectors([[1]])
        assert v

        v = Vectors([[0, 1, 2, 3], [4, 5, 6, 7], [1, 2, 3, 4]])
        assert v

        try:
            v = Vectors([[0, 1, 2, 3], [4, 5, 6, 7], [1, 2, 3]])
            assert False
        except:
            assert True

        try:
            v = Vectors([[0, 1, 2, 3], [4, 5, 6, 7], [1.2, 2, 3]])
            assert False
        except:
            assert True


    def test_vectors_file(self):
        """test vector constructor from file"""

        v1 = Vectors("vectors.vec")
        v1.save("vectors2.vec", ViewPoint="Data")

        v2 = Vectors("vectors2.vec")

        assert v1 and v2
        assert len(v2) == len(v1)

        for i in xrange(len(v2)):
            assert v1[i] == v2[i]



    def test_vectors_display(self):
        """ASCII representation"""
        v = Vectors([[0, 1, 2, 3], [4, 5, 6, 7]])
        s = str(v)
        assert v.display() == s



    def test_vectors_container(self):
        """vector container : len"""
        v = Vectors([[0,1,2,3], [4,5,6,7]])
        assert len(v) == 2

        for i in v:
            assert len(i) == 4

        assert v[0] == range(4)
        assert v[1][1] == 5


    def test_vector_distance(self):
        """ test vector distance"""
        v = VectorDistance('N', 'O', 'S') 
        assert v and len(v) == 3

        v = VectorDistance('NUMERIC', 'ORDINAL', 'SYMBOLIC')
        assert v and len(v) == 3

        v = VectorDistance(2.3, 'N', 4, 'O', 6, 'S')
        assert v and len(v) == 3

        v = VectorDistance( (2.3, 'N'),  (4, 'O'), (6, 'S'))
        assert v and len(v) == 3

        v = VectorDistance(2.3, 'N', 4, 'O', 6, 'S', distance = "QUADRATIC")
        assert v and len(v) == 3

        v = VectorDistance('NUMERIC', 'ORDINAL', 'SYMBOLIC', \
                           Distance="QUADRATIC")
        assert v and len(v) == 3

        assert str(VectorDistance('N', 'O', 'S'))


    def test_variance_analysis(self):
        """test VarianceAnalysis"""
        vec10 = Vectors("chene_sessile.vec")
        va = VarianceAnalysis(vec10, 1, 4, "O")
        assert va and str(va)


    def test_contingency_table(self):
        """test contingency table"""
        vec10 = Vectors("chene_sessile.vec")
        ct = ContingencyTable(vec10, 1, 4)
        assert ct and str(ct)
        
    def test_to_clean(self):
         #########################################################################
         #
         #  vectors : contingency table, one-way variance analysis,
         #  linear regression or nonparametric regression (loess smoother)
         #
         #  Oak trunk annual shoots
        #
        #  INDEX_PARAMETER : TIME  (year of growth - 1995, 1996, 1997)
        #  VARIABLE 1 : length of the annual shoot (cm)
        #  VARIABLE 2 : diameter of the annual shoot (1/10 de mm)
        #  VARIABLE 3 : number of cycles
        #  VARIABLE 4 : number of nodes
        #  VARIABLE 5 : number de branches
        #
        #########################################################################
        vec10 = Vectors("chene_sessile.vec")
    
        # plot of the pointwise averages
        # Plot(Regression(vec10, "MovingAverage", 1, 2, [1]))
    
        vec95 = ValueSelect(vec10, 1, 1995)
        vec96 = ValueSelect(vec10, 1, 1996)
        #vec97 = ValueSelect(vec10, 1, 1997)

        VarianceAnalysis(vec10, 1, 2, "N")
        Compare(ExtractHistogram(vec95, 2), ExtractHistogram(vec96, 2), ExtractHistogram(vec95, 2), "N")
        # Plot(ExtractHistogram(vec95, 2), ExtractHistogram(vec96, 2), ExtractHistogram(vec97, 2))
    
        ContingencyTable(vec10, 1, 4)
    
        # one-way variance analysis based on ranks
    
        VarianceAnalysis(vec10, 1, 4, "O")
        Compare(ExtractHistogram(vec95, 4), ExtractHistogram(vec96, 4), ExtractHistogram(vec95, 4), "O")
        # Plot(ExtractHistogram(vec95, 4), ExtractHistogram(vec96, 4), ExtractHistogram(vec97, 4))
    
        # Plot(ExtractHistogram(vec95, 5), ExtractHistogram(vec96, 5), ExtractHistogram(vec97, 5))
        # Plot(ExtractHistogram(vec95, 6), ExtractHistogram(vec96, 6), ExtractHistogram(vec97, 6))
    
        vec11 = ValueSelect(vec10, 4, 1)
        vec12 = ValueSelect(vec10, 4, 2)
        vec13 = ValueSelect(vec10, 4, 3, 4)
    
        # Plot(ExtractHistogram(vec11, 2), ExtractHistogram(vec12, 2), ExtractHistogram(vec13, 2))
        # Plot(ExtractHistogram(vec11, 5), ExtractHistogram(vec12, 5), ExtractHistogram(vec13, 5))
    
        mixt20 = Estimate(ExtractHistogram(vec10, 2), "MIXTURE", "NB", "NB", "NB", "NB", NbComponent="Estimated")
        # Display(mixt20)
        # Plot(mixt20)
        # Plot(ExtractDistribution(mixt20, "Mixture"))
    
        mixt21 = Estimate(ExtractHistogram(vec10, 5), "MIXTURE", "NB", "NB", "NB", "NB", NbComponent="Estimated")
    
        vec9596 = ValueSelect(vec10, 1, 1995, 1996)
        # Plot(ExtractHistogram(ValueSelect(vec9596, 4, 1), 6), ExtractHistogram(ValueSelect(vec9596, 4, 2), 6), ExtractHistogram(ValueSelect(vec9596, 4, 3, 4), 6))
    
        # linear regression
    
        regress10 = Regression(vec10, "Linear", 5, 2)
        Display(regress10)
        # Plot(regress10)
    
        # nonparametric regression (loess smoother)

        regress11 = Regression(vec10, "NearestNeighbours",  5, 2, 0.3)
    
        regress12 = Regression(vec9596, "Linear", 5, 6)
        regress13 = Regression(vec9596, "NearestNeighbours", 5, 6, 0.5)
        
        vec15 = SelectVariable(vec10, [1, 3, 6], Mode="Reject")


        #########################################################################
        #
        #  Distance matrix and clustering (partitioning or hierarchical methods) 
        #
        #########################################################################

        # computation of a distance matrix using a standardization procedure

        matrix10 = Compare(vec15, VectorDistance("N", "N", "N"))
        
        # clustering using a partitioning method
    
        # Display(Clustering(matrix10, "Partition", 2))

        vec151 = SelectIndividual(vec10,  [69, 48, 41, 44, 32, 47, 81, 95, 11, 36, 75, 108, 56, 83, 38, 98, 113, 134, 110, 101, 77, 35, 74, 80, 50, 24, 89, 128, 5, 45, 8, 116, 119, 132, 61, 78, 53, 29, 131, 65, 90, 96, 104, 20, 86, 66, 42, 68, 125, 14, 23, 54, 33, 26, 71, 129, 102, 51, 70, 111, 138, 19, 127, 62, 117, 137, 2, 28, 17])
        vec152 = SelectIndividual(vec10, [100, 13, 133, 105, 72, 9, 93, 109, 30, 115, 63, 7, 55, 37, 15, 114, 106, 46, 73, 18, 3, 87, 58, 43, 60, 76, 52, 6, 39, 31, 12, 99, 121, 123, 22, 79, 94, 88, 21, 97, 25, 40, 57, 136, 67, 49, 10, 4, 120, 92, 27, 91, 64, 124, 16, 130, 84, 107, 126, 103, 122, 112, 59, 1, 82, 34, 135, 118, 85])
        # Plot(ExtractHistogram(vec151, 4), ExtractHistogram(vec152, 4))

        matrix11 = Compare(vec15, VectorDistance("N", "O", "N"))
        
        Clustering(matrix10, "Hierarchy", Algorithm="Agglomerative")
        Clustering(matrix10, "Hierarchy", Algorithm="Divisive")
        
        vec16 = SelectVariable(vec9596, [1, 3], Mode="Reject")
        matrix12 = Compare(vec16, VectorDistance("N", "N", "N", "N"))
        matrix13 = Compare(vec16, VectorDistance("N", "O", "N", "N"))
    
        

if __name__=="__main__":
    # perform all the test in the class Test (unit tests)
    test = Test()
    for method in dir(test):
        if method.startswith('_'):
            continue
        if callable(getattr(test, method)):
            getattr(test, method)()
        else:
            print 'skipping'
    # and functional tests.    
    
