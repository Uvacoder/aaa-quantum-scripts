#include "qiga.h"

#define Qij (Q[i * chromlen + j])

void QIGA::initialize() {
  int i, j;
  for (i = 0; i < popsize; i++) {
    for (j = 0; j < chromlen; j++) {
      Qij = M_PI_4;
    }
  }
}

void QIGA::observe() {
  int i, j ;
  for (i = 0; i < popsize; i++) {
    for (j = 0; j < chromlen; j++) {
      float alpha = cos(Qij);
      float r = 1.0f * rand() / (RAND_MAX + 1.0);
      P[i][j] = (r < alpha * alpha) ? '0' : '1';
    }
  }
}

void QIGA::repair() {
  for (int i = 0; i < popsize; i++) {
    problem -> repair(P[i], chromlen);
  }
}

void QIGA::evaluate() {
  int i, j;
  for (i = 0; i < popsize; i++) {
    fvals[i] = problem -> evaluate(P[i], chromlen);
  }
}

void QIGA::update() {
  int i, j;
  int fxGtfb;
  float delta;
  float rot;
  for (i = 0; i < popsize; i++) {
    fxGtfb = fvals[i] >= bestval;
    for (j = 0; j < chromlen; j++) {
      int x = P[i][j];
      int b = best[j];
      delta = lookup_table[x == '1'][b == '1'][fxGtfb];
      float cangle = fmodf(Qij, M_PI);
      
      int sindex = 
        (cangle > EPSILON && cangle < M_PI_2 - EPSILON) ? 0 :
        
        // if alpha * beta < 0
        (cangle < M_PI - EPSILON && cangle > M_PI_2 + EPSILON) ? 1:

        // if alpha == 0
        (fabsl((fmodf((M_PI_2 + Qij), M_PI) - M_PI_2)) < EPSILON) ? 2:

        3;
      
      float sign = sign_table[x == '1'][b == '1'][fxGtfb][sindex];
      Qij += sign * delta;
    }
  }
}

void QIGA::storebest() {
  int i;
  float val = -1;
  char *b;
  for (i = 0; i < popsize; i++) {
    if (fvals[i] > val) {
      val = fvals[i];
      b = P[i];
    }
  }

  if (val > bestval) {
    bestval = val;
    memcpy(best, b, chromlen);
  }
}

void QIGA::qiga() {
  int t = 0;
  bestval = -1;
  initialize();
  observe();
  repair();
  evaluate();
  storebest();

  while (t < tmax) {
    observe();
    repair();
    evaluate();
    update();
    storebest();

    t++;
  }

  printf("best solution: ");
  fwrite(best, 1, chromlen, stdout);
  printf("\nfitness: %f\n", bestval);
  fflush(stdout);
}

