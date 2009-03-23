"""Distribution tests"""

#from openalea.stat_tool import *
from openalea.stat_tool import _stat_tool
from openalea.stat_tool.distribution import Distribution
from openalea.stat_tool.distribution import Uniform
from openalea.stat_tool.distribution import Binomial
from openalea.stat_tool.distribution import NegativeBinomial
from openalea.stat_tool.distribution import Poisson
from openalea.stat_tool.distribution import ToHistogram
from openalea.stat_tool.distribution import ToDistribution
from openalea.stat_tool.histogram import Histogram


class TestDistribution:
    """unit tests"""
    def __init__(self):
        pass
    
    def test_empty(self):
        """Test that empty constructor fails"""
        try:
            _m = Distribution()
            assert False
        except Exception:
            assert True
    
    def test_file(self):
        """run constructor with filename argument"""
        m = Distribution("distribution1.dist")
        assert m
        
                 
    def test_display(self):
        """test display"""
        d = Distribution("UNIFORM", 0, 10)

        s = str(d)
        assert d.display() == s

    def test_uniform(self):

        d = Distribution("UNIFORM", 0, 10)
        assert list(d.simulate(1000))

        d = Uniform(0, 10)
        assert list(d.simulate(1000))

        m = d.simulate(1000).extract_model()
        assert isinstance(m, _stat_tool._ParametricModel)

    def test_binomial(self):

        d = Distribution("BINOMIAL", 0, 10, 0.5)
        assert list(d.simulate(1000))

        d = Binomial(0, 10, 0.5)
        assert list(d.simulate(1000))

        m = d.simulate(1000).extract_model()
        assert isinstance(m, _stat_tool._ParametricModel)

    def test_poisson(self):

        d = Distribution("POISSON", 0, 2)
        assert list(d.simulate(1000))

        d = Poisson(0, 2)
        assert list(d.simulate(1000))

        m = d.simulate(1000).extract_model()
        assert isinstance(m, _stat_tool._ParametricModel)

    def test_neg_binomial(self):
        
        d = Distribution("NEGATIVE_BINOMIAL", 0, 1, 0.5)
        assert list(d.simulate(1000))

        d = NegativeBinomial(0, 1, 0.5)
        assert list(d.simulate(1000))

        m = d.simulate(1000).extract_model()
        assert isinstance(m, _stat_tool._ParametricModel)

    def test_plot(self):

        d = Distribution("POISSON", 0, 2)
        d.plot()

    def test_fromnothing(self):

        # From nothing
        try:
            d = Distribution()
            assert False

        except Exception:
            assert True

    def test_to_histogram(self):

        d = Distribution("NEGATIVE_BINOMIAL", 0, 1, 0.5)
        h = d.simulate(1000)
        d2 = ToDistribution(h)
        assert h and d2

        h2 = ToHistogram(d2)
        assert h2
        assert h == h2



def test1():
     #########################################################################
    #
    #  discrete distributions/histograms, comparison of histograms/frequency distributions
    #
    #  beech, Wild cherry tree: number of nodes per growth unit (GU)
    #
    #  meri1.his: order 1,
    #  meri1.his: order 2,
    #  meri1.his: order 3, GU 1,
    #  meri1.his: order 3, GU 2,
    #  meri5.his: short shoots.
    #
    #########################################################################
    from openalea.stat_tool import Display, Plot, Simulate, Estimate, Cluster, Transcode, ValueSelect, Shift,Compare, ComparisonTest, Fit

    dist0 = Distribution("NEGATIVE_BINOMIAL", 0, 1, 0.3)
    dist0 = Distribution("distribution1.dist")

    dist1 = Distribution("B", 0, 10, 0.3)
    dist1 = Distribution("NB", 0, 3.5, 0.3)

    # computation of survivior function and hasard rate

    Display(dist1, ViewPoint="Survival")
    Plot(dist1, ViewPoint="Survival")

    # simulation/estimation

    histo1 = Simulate(dist1, 200)

    #Display(histo1, ViewPoint->"Survival")
    #Plot(histo1, ViewPoint->"Survival")

    # "B" "BINOMIAL", "P" "POISSON", "NB" "NEGATIVE_BINOMIAL"
    # InfBoundStatus->"Free" (default) / "Fixed"
    # MinInfBound->0 (default) / 1

    dist2 = Estimate(histo1, "NB", MinInfBound=0, InfBoundStatus="Fixed")

    fagus = Histogram("fagus1.his")

    # transformation of histograms, extraction/filter

    histo2 = Cluster(fagus, "Step", 2)
    histo3 = Cluster(fagus, "Information", 0.8)
    histo4 = Cluster(fagus, "Limit", [2, 4, 6, 8, 10])
    histo5 = Transcode(fagus, [1, 2, 2, 3, 3, 4, 4, 5])
    Display(histo5, Detail=2)

    histo7 = Shift(fagus, -2)

    histo8 = ValueSelect(fagus, 2, 8)

    dist3 = Estimate(fagus, "B")

    # comparison of histograms

    meri1 = Histogram("meri1.his")
    meri2 = Histogram("meri2.his")
    meri3 = Histogram("meri3.his")
    meri4 = Histogram("meri4.his")
    meri5 = Histogram("meri5.his")

    ## Compare(meri1, meri2, meri3, meri4, meri5, "N", FileName="ASCII/meri.cmp")
    Compare(meri1, meri2, meri3, meri4, meri5, "O")

    ComparisonTest("F", meri1, meri2)
    ComparisonTest("T", meri1, meri2)
    ComparisonTest("W", meri1, meri2)

    # fit of a known distribution to an  histogram

    dist5 = Fit(meri5, Distribution("B", 0, 10, 0.437879))
    Display(dist5, Detail=2)
    Plot(dist5)
    
    
    
