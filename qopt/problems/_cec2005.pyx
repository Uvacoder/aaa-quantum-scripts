
# http://www.mail-archive.com/cython-dev@codespeak.net/msg07189.html

import ctypes
import os

from qopt.framework cimport Problem, ProblemDouble, ProblemCpp

cdef extern from "cec2005.h":
    cdef cppclass CEC2005cpp "CEC2005" (ProblemCpp[double,double]):
        CEC2005cpp(int fnum) except +SyntaxError

cdef class CEC2005(ProblemDouble):
    cdef long double (*r_evaluator) (long double *x,int n)
    def __cinit__(self, int fnum):
        lib = ctypes.CDLL(os.path.dirname(os.path.realpath(__file__))+'/CEC2005/libf%d.so' % fnum)
        self.r_evaluator = (<long double (**)(long double *, int)><size_t>ctypes.addressof(lib.evaluate))[0]
        #self.thisptr = new CEC2005cpp(fnum)
    def evaluate(self, x):
        cdef long double tab[50]
        for i in xrange(len(x)):
            tab[i] = x[i]
        return self.r_evaluator(tab, len(x))


