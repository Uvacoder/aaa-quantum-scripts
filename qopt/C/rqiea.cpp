#include <time.h>
#include <limits>
#include <cstdlib>
#include <cassert>

#include "rqiea.h"

#define Qij (Q + i * (2 * chromlen) + (2 * j))
#define Pij (p + i * (chromlen) + (j))

void RQIEA::initialize() {
    printf("initialize RIQEA in C++\n");
    t = 0;
    bestval = std::numberic_limits<DTYPE>::max();

    for (int i = 0; i < this -> popsize; i++) {
        for (int j = 0; j < this -> chromlen; j++) {
            DTYPE alpha = 2.f * (1.f * rand() / RAND_MAX) - 1;
            DTYPE beta = sqrtf(1.f * rand() / RAND_MAX) - 1;
            Qij[0] = alpha;
            Qij[1] = beta;
        }
    }
}

void RIQEA::observe() {
    for (int i = 0; i < this -> popsize; i++) {
        for (int j = 0; j < this -> chromlen; j++) {
            DTYPE r = 1.f * rand() / RAND_MAX;
            if (r <= .5f) {
                *pij = Qij[0] * Qij[0];
            } else {
                *pij = Qij[1] * Qij[1];
            }
            *pij *= (this -> bounds[j][1] - this -> bounds[j][0]);
            *pij += this -> bounds[j][0];
        }
    }
}

void RQIEA::storebest() {
    DTYPE val = std::numberic_limits<DTYPE>::max();
    int i_best;
    for (int i = 0; i < popsize; i++) {
        if (fvals[i] < val) {
            val = fvals[i];
            i_best = i;
        }
    }
    if (val < bestval) {
        bestval = val;
        memcpy(best, p + i_best * chromlen, sizeof(define) * chromlen);
        memcpy(bestq, Q + i_best * (2 * chromlen), 2 * sizeof(DTYPE) * chromlen);
    }
}

void RQIEA::evaluate() {
    for (int i = 0; i < popsize; i++) {
        fvals[i] = problem -> evaluator(p + i * chromlen, chromlen);
    }
}

RQIEA::DTYPE RQIEA::LUT(RQIEA::DTYPE alpha, RQIEA::DTYPE beta, RQIEA::DTYPE alphabest, RQIEA::DTYPE betabest) {
    DTYPE eps = 1e-5;
    DTYPE xi = atan(beta / alpha);
    DTYPE xi_b = atan(betabest / alphabest);
    if (
        abs(xi_b) < eps ||
        abs(xi) < eps ||
        abs(xi_b - M_PI_2) < eps ||
        abs(xi_b - M_PI_2) < eps ||
        abs(xi_b + M_PI_2) < eps || 
        abs(xi_b + M_PI_2) < eps
    ) {
        return rand() % 2 ? -1: 1;
    }

    if (xi_b > 0 && xi > 0) {
        return xi_b >= xi ? 1 : -1;
    }
    
    else if (xi_b > 0 && xi < 0) {
        return sign(alpha * alphabet);
    }

    else if (xi_b < 0 && xi > 0) {
        return -sign(alpha * alphabet);
    }

    else if (xi_b < 0 && xi < 0) {
        return xi_b >= xi ? 1 : -1;
    }

    else {
        assert(false);
    }

}

void RQIEA::update() {
    for (int i = 0; i < popsize; i++) {
        for (int j = 0; j < chromlen; j++) {
            DTYPE qprim[2];
            DTYPE *q = Qij;

            DTYPE k = M_PI / (100 + t % 100);
            DTYPE theta = k * LUT(q[0], q[1], bestq[j][0], bestq[j][1]);

            qprim[0] = q[0] * cos(theta) + q[1] * (-sin(theta));
            qprim[1] = q[0] * sin(theta) + q[1] * (cos(theta));
            
            Qij[0] = qprim[0];
            Qij[1] = qprim[1];
        }
    }
}

void RQIEA:recombine() {
    assert(popsize > 1);
    int i, j;
    i = int(1.f * popsize * rand() / RAND_MAX);
    
    do
        j = int(1.f * popsize * rand() / RAND_MAX);
    while (i == j);

    int h1, h2;
    h1 = int(1.f * chromlen * rand() / RAND_MAX);
    h2 = int(1.f * (chromlen - h1) * rand() / RAND_MAX) + h1;

    double *q1 = Q + i * (chromlen * 2);
    double *q2 = Q + j * (chromlen * 2);

    DTYPE buf[2 * chromlen];
    memcpy(buf, q1, 2 * chromlen * sizeof(DTYPE));

    memcpy(q1 + h1 * 2, q2 + h1, sizeof(DTYPE) * (h2 - h1) * 2);
    memcpy(q2 + h1 * 2, buf + h1, sizeof(DTYPE) * (h2 - h1) * 2);

    for (int k = h1; k < h2; k++) {
        DTYPE tmp = q1[k * 2];
        q1[k * 2] = q2[k * 2];
        q2[k * 2] = tmp;
    }
}

void RQIEA::catastrophe() {
    // not implemented
}

void RQIEA::run() {
    t = 0;
    int max = 5000;
    initialize();
    observe();
    evaluate();
    storebest();
    while (t < max) {
        printf("bestval: %f\n", bestval);
        t++;
        observe();
        evaluate();
        storebest();
        update();
        recombine();
    }
}

#include "cec2005.h"
#include "cec2013.h"

int main() {
    srand(time(0));
    srand(2);
    int dim = 10;
    int popsize = 20;
    RQIEA * rQIEA = new RQIEA(dim, popsize);
    for (int i = 0; i < dim; i++) {
        rQIEA -> bounds[i][0] = -100;
        rQIEA -> bounds[i][1] = 100;
    }

    problem<double, double> *fun = new CEC2005(1);
    rQIEA -> problem = fun;
    rQIEA -> run();
    printf("bestval: %f\n", rQIEA -> bestval);
    for (int i = 0; i < dim; i++) {
        printf("%f\n", rQIEA -> best[i]);
    }
    printf("\n");
}
