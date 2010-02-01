/* -*-c++-*-
 *  ----------------------------------------------------------------------------
 *
 *       V-Plants: Exploring and Modeling Plant Architecture
 *
 *       Copyright 1995-2010 CIRAD/INRIA Virtual Plants
 *
 *       File author(s): Y. Guedon (yann.guedon@cirad.fr)
 *
 *       $Source$
 *       $Id$
 *
 *       Forum for V-Plants developers:
 *
 *  ----------------------------------------------------------------------------
 *
 *                      GNU General Public Licence
 *
 *       This program is free software; you can redistribute it and/or
 *       modify it under the terms of the GNU General Public License as
 *       published by the Free Software Foundation; either version 2 of
 *       the License, or (at your option) any later version.
 *
 *       This program is distributed in the hope that it will be useful,
 *       but WITHOUT ANY WARRANTY; without even the implied warranty of
 *       MERCHANTABILITY or FITNESS For A PARTICULAR PURPOSE. See the
 *       GNU General Public License for more details.
 *
 *       You should have received a copy of the GNU General Public
 *       License along with this program; see the file COPYING. If not,
 *       write to the Free Software Foundation, Inc., 59
 *       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ----------------------------------------------------------------------------
 */



#include <math.h>
#include "stat_tools.h"
#include "compound.h"
#include "stat_label.h"

using namespace std;



/*--------------------------------------------------------------*
 *
 *  Calcul d'une loi composee.
 *
 *  arguments : nombre minimum de valeurs, seuil sur la fonction de repartition,
 *              flags pour le calcul de la loi de la somme et
 *              pour le calcul de la loi elementaire.
 *
 *--------------------------------------------------------------*/

void Compound::computation(int min_nb_value , double cumul_threshold ,
                           bool sum_flag , bool dist_flag)

{
  register int i , j;
  double *smass , *pmass , *cmass;
  Parametric *power_dist;


  // calcul de la loi de la somme et de la loi elementaire

  if (sum_flag) {
    sum_distribution->computation(1 , CUMUL_THRESHOLD);
  }
  if (dist_flag) {
    distribution->computation(min_nb_value , cumul_threshold);
  }

  // calcul des puissances de convolution de la loi elementaire et
  // calcul de la loi composee resultante

  power_dist = new Parametric((sum_distribution->nb_value - 1) * (distribution->nb_value - 1) + 1 ,
                              distribution->ident);

  pmass = mass;
  for (i = 0;i < MIN(power_dist->nb_value , alloc_nb_value);i++) {
    *pmass++ = 0.;
  }
  if (sum_distribution->offset == 0) {
    *mass += sum_distribution->mass[0];
  }

  if (((distribution->ident == NONPARAMETRIC) || (distribution->ident == UNIFORM)) &&
      (sum_distribution->offset > 1)) {
    power_dist->mass_copy(*distribution);
    for (i = 2;i < sum_distribution->offset;i++) {
      power_dist->convolution(*power_dist , *distribution);
    }
  }

  smass = sum_distribution->mass + MAX(sum_distribution->offset , 1);
  for (i = MAX(sum_distribution->offset , 1);i < sum_distribution->nb_value;i++) {
    if (i == 1) {
      power_dist->mass_copy(*distribution);
    }

    else {
      if ((distribution->ident == NONPARAMETRIC) || (distribution->ident == UNIFORM)) {
        power_dist->convolution(*power_dist , *distribution);
      }
 
      else {
        power_dist->inf_bound = i * distribution->inf_bound;
        power_dist->sup_bound = (distribution->sup_bound != I_DEFAULT ? i * distribution->sup_bound : I_DEFAULT);
        power_dist->parameter = (distribution->parameter != D_DEFAULT ? i * distribution->parameter : D_DEFAULT);
        power_dist->probability = distribution->probability;

        power_dist->computation(min_nb_value , cumul_threshold);
      }
    }

    pmass = mass + power_dist->offset;
    cmass = power_dist->mass + power_dist->offset;
    for (j = power_dist->offset;j < MIN(power_dist->nb_value , alloc_nb_value);j++) {
      *pmass++ += *smass * *cmass++;
    }
    smass++;
  }

  offset_computation();
  nb_value = MIN(power_dist->nb_value , alloc_nb_value);

  cumul_computation();

  max_computation();
  mean_computation();
  variance_computation();

  delete power_dist;
}


/*--------------------------------------------------------------*
 *
 *  Calcul d'une loi composee.
 *
 *  arguments : pointeur sur les puissances de convolution de la loi elementaire,
 *              nombre minimum de valeurs, seuil sur la fonction de repartition,
 *              flags pour le calcul de la loi de la somme et
 *              pour le calcul de la loi elementaire.
 *
 *--------------------------------------------------------------*/

void Compound::computation(Parametric **power_dist , int min_nb_value ,
                           double cumul_threshold , bool sum_flag , bool dist_flag)

{
  register int i , j;
  int sum_nb_value , min;
  double *smass , *pmass , *cmass;


  // calcul de la loi de la somme et de la loi elementaire

  sum_nb_value = sum_distribution->nb_value;
  if (sum_flag) {
    sum_distribution->computation(1 , CUMUL_THRESHOLD);
  }
  if (dist_flag) {
    distribution->computation(min_nb_value , cumul_threshold);
  }

  // calcul des puissance de convolution de la loi elementaire

  if (dist_flag) {
    min = MAX(sum_distribution->offset - 1 , 1);
  }
  else {
    min = sum_nb_value;
  }

  if ((distribution->ident == NONPARAMETRIC) || (distribution->ident == UNIFORM)) {
    power_dist[min]->ident = distribution->ident;

    if (dist_flag) {
      power_dist[min]->mass_copy(*distribution);
      for (i = 2;i <= min;i++) {
        power_dist[min]->convolution(*power_dist[min] , *distribution);
      }
    }

    else if (min < sum_distribution->nb_value) {
      power_dist[min]->convolution(*power_dist[min - 1] , *distribution);
    }

    for (i = min + 1;i < sum_distribution->nb_value;i++) {
      power_dist[i]->ident = distribution->ident;
      power_dist[i]->convolution(*power_dist[i - 1] , *distribution);
    }
  }

  else {
    if (min == 1) {
      power_dist[min]->mass_copy(*distribution);
      min++;
    }

    for (i = min;i < sum_distribution->nb_value;i++) {
      power_dist[i]->ident = distribution->ident;
      power_dist[i]->inf_bound = i * distribution->inf_bound;
      power_dist[i]->sup_bound = (distribution->sup_bound != I_DEFAULT ? i * distribution->sup_bound : I_DEFAULT);
      power_dist[i]->parameter = (distribution->parameter != D_DEFAULT ? i * distribution->parameter : D_DEFAULT);
      power_dist[i]->probability = distribution->probability;

      power_dist[i]->computation(min_nb_value , cumul_threshold);
    }
  }

  // calcul de la loi composee resultante

  if (sum_distribution->offset == 0) {
    offset = 0;
  }
  else {
    offset = power_dist[sum_distribution->offset]->offset;
  }
  nb_value = MIN(power_dist[sum_distribution->nb_value - 1]->nb_value , alloc_nb_value);

  pmass = mass;
  for (i = 0;i < nb_value;i++) {
    *pmass++ = 0.;
  }
  if (sum_distribution->offset == 0) {
    *mass += sum_distribution->mass[0];
  }

  smass = sum_distribution->mass + MAX(sum_distribution->offset , 1);
  for (i = MAX(sum_distribution->offset , 1);i < sum_distribution->nb_value;i++) {
    pmass = mass + power_dist[i]->offset;
    cmass = power_dist[i]->mass + power_dist[i]->offset;
    for (j = power_dist[i]->offset;j < power_dist[i]->nb_value;j++) {
      *pmass++ += *smass * *cmass++;
    }
    smass++;
  }

  cumul_computation();

  max_computation();
  mean_computation();
  variance_computation();
}


/*--------------------------------------------------------------*
 *
 *  Calcul des quantites de reestimation (estimateur EM).
 *
 *  arguments : reference sur l'histogramme,
 *              pointeurs sur les puissances de convolution
 *              de la loi elementaire et sur les quantites de reestimation.
 *
 *--------------------------------------------------------------*/

void Compound::expectation_step(const Histogram &histo , Parametric **power_dist ,
                                Reestimation<double> *sum_reestim , Reestimation<double> *reestim) const

{
  register int i , j , k;
  int *pfrequency;
  double num , denom , *rfrequency , *term , *pterm , *smass , *pmass;


  term = new double[sum_distribution->nb_value];

  // initialisation

  if (sum_reestim) {
    rfrequency = sum_reestim->frequency;
    for (i = 0;i < sum_reestim->alloc_nb_value;i++) {
      *rfrequency++ = 0.;
    }
  }

  if (reestim) {
    rfrequency = reestim->frequency;
    for (i = 0;i < reestim->alloc_nb_value;i++) {
      *rfrequency++ = 0.;
    }
  }

  pfrequency = histo.frequency + histo.offset;
  for (i = histo.offset;i < histo.nb_value;i++) {
    if (*pfrequency > 0) {

      // calcul du denominateur

      pterm = term + sum_distribution->offset;
      smass = sum_distribution->mass + sum_distribution->offset;
      denom = 0.;

      if (sum_distribution->offset == 0) {
        if (i == 0) {
          *pterm = *smass;
          denom += *pterm++;
        }
        else {
          *pterm++ = 0.;
        }
        smass++;
      }

      for (j = MAX(sum_distribution->offset , 1);j < sum_distribution->nb_value;j++) {
        if ((i >= power_dist[j]->offset) && (i < power_dist[j]->nb_value)) {
          *pterm = *smass * power_dist[j]->mass[i];
          denom += *pterm++;
        }
        else {
          *pterm++ = 0.;
        }
        smass++;
      }

      if (denom > 0.) {

        // accumulation des quantites de reestimation de la loi de la somme

        if (sum_reestim) {
          rfrequency = sum_reestim->frequency + sum_distribution->offset;
          pterm = term + sum_distribution->offset;
          for (j = sum_distribution->offset;j < sum_distribution->nb_value;j++) {
            *rfrequency++ += *pfrequency * *pterm++ / denom;
          }
        }

        // accumulation des quantites de reestimation de la loi elementaire

        if (reestim) {
          rfrequency = reestim->frequency + distribution->offset;
          pmass = distribution->mass + distribution->offset;

          for (j = distribution->offset;j <= MIN(i , distribution->nb_value - 1);j++) {
            smass = sum_distribution->mass + sum_distribution->nb_value;
            num = 0.;

            for (k = sum_distribution->nb_value - 1;k >= MAX(sum_distribution->offset , 1);k--) {
              smass--;

              if (k == 1) {
                if (i == j) {
                  num += *smass;
                }
              }

              else {
                if (i - j < power_dist[k - 1]->nb_value) {
                  if (i - j >= power_dist[k - 1]->offset) {
                    num += *smass * k * power_dist[k - 1]->mass[i - j];
                  }
                }
                else {
                  break;
                }
              }
            }

            *rfrequency++ += *pfrequency * *pmass++ * num / denom;
          }
        }

      }
    }
    pfrequency++;
  }

  delete [] term;

  if (sum_reestim) {
    sum_reestim->nb_value_computation();
    sum_reestim->offset_computation();
    sum_reestim->nb_element_computation();
    sum_reestim->max_computation();
    sum_reestim->mean_computation();
    sum_reestim->variance_computation();
  }

  if (reestim) {
    reestim->nb_value_computation();
    reestim->offset_computation();
    reestim->nb_element_computation();
    reestim->max_computation();
    reestim->mean_computation();
    reestim->variance_computation();
  }

# ifdef DEBUG
  if (sum_reestim) {
    cout << "\nquantites de reestimation loi de la somme :" << *sum_reestim << endl;
  }
  if (reestim) {
    cout << "\nquantites de reestimation loi elementaire :" << *reestim << endl;
  }
# endif

}


/*--------------------------------------------------------------*
 *
 *  Estimation des parametres d'une loi composee par l'algorithme EM.
 *
 *  arguments : references sur un objet Format_error, stream, references sur les lois,
 *              type de la loi inconnue ('s' : loi de la somme, 'e' : loi elementaire),
 *              type d'estimateur (vraisemblance, vraisemblance penalisee ou
 *              estimation d'une loi parametrique), nombre d'iterations,
 *              poids de la penalisation, type de penalisation,
 *              type de gestion des effets de bord (zero a l'exterieur du support ou
 *              prolongation de la loi).
 *
 *--------------------------------------------------------------*/

Compound* Histogram::compound_estimation(Format_error &error , ostream &os , const Parametric &sum_dist ,
                                         const Parametric &dist , char type , int estimator ,
                                         int nb_iter , double weight , int penalty_type ,
                                         int outside) const

{
  bool status = true , sum_compute , dist_compute;
  register int i;
  int sum_nb_value , nb_likelihood_decrease;
  double likelihood , previous_likelihood , hlikelihood , *penalty;
  Parametric **power_dist;
  Reestimation<double> *sum_reestim , *reestim;
  Compound *compound;
  Compound_data *compound_histo;


  compound = NULL;
  error.init();

  if ((nb_iter != I_DEFAULT) && (nb_iter < 1)) {
    status = false;
    error.update(STAT_error[STATR_NB_ITERATION]);
  }

  if ((weight != D_DEFAULT) && (weight <= 0.)) {
    status = false;
    error.update(STAT_error[STATR_PENALTY_WEIGHT]);
  }

  if (status) {

    // creation d'un objet Compound

    compound = new Compound(sum_dist , dist , type);
    compound->compound_data = new Compound_data(*this , *compound);
    compound_histo = compound->compound_data;

    if (estimator == PENALIZED_LIKELIHOOD) {
      switch (type) {
      case 's' :
        penalty = new double[compound->sum_distribution->alloc_nb_value];
        break;
      case 'e' :
        penalty = new double[compound->distribution->alloc_nb_value];
        break;
      }

      if (weight == D_DEFAULT) {
        if (penalty_type != ENTROPY) {
          weight = COMPOUND_DIFFERENCE_WEIGHT;
        }
        else {
          weight = COMPOUND_ENTROPY_WEIGHT;
        }
      }

      switch (type) {
      case 's' :
        weight *= nb_element;
        break;
      case 'e' :
        weight *= compound->sum_distribution->mean * nb_element;
        break;
      }
    }

    switch (type) {
    case 's' :
      sum_compute = true;
      dist_compute = false;
      break;
    case 'e' :
      sum_compute = false;
      dist_compute = true;
      break;
    }

    // creation des puissances de convolution de la loi elementaire et
    // des quantites de reestimation

    sum_nb_value = compound->sum_distribution->alloc_nb_value;

    power_dist = new Parametric*[sum_nb_value];
    for (i = MAX(sum_dist.offset - 1 , 1);i < sum_nb_value;i++) {
      power_dist[i] = new Parametric(i * (compound->distribution->alloc_nb_value - 1) + 1 ,
                                     compound->distribution->ident);
    }

    sum_reestim = new Reestimation<double>(sum_nb_value);
    reestim = new Reestimation<double>(compound->distribution->alloc_nb_value);

    compound->computation(power_dist , nb_value , COMPOUND_THRESHOLD ,
                          sum_compute , true);

#   ifdef DEBUG
    cout << " (" << compound->mean << " " << compound->variance << ")" << endl;
#   endif

    switch (type) {
    case 's' :
      compound->sum_distribution->init(NONPARAMETRIC , I_DEFAULT , I_DEFAULT , D_DEFAULT , D_DEFAULT);
      break;
    case 'e' :
      compound->distribution->init(NONPARAMETRIC , I_DEFAULT , I_DEFAULT , D_DEFAULT , D_DEFAULT);
      break;
    }

    likelihood = D_INF;
    i = 0;

    do {
      i++;

      // calcul des quantites de reestimation

      switch (type) {

      case 's' : {
        compound->expectation_step(*this , power_dist , sum_reestim , 0);

        if (estimator != PENALIZED_LIKELIHOOD) {
          sum_reestim->distribution_estimation(compound->sum_distribution);
        }
        else {
          sum_reestim->penalized_likelihood_estimation(compound->sum_distribution , weight ,
                                                       penalty_type , penalty , outside);
        }
        break;
      }

      case 'e' : {
        compound->expectation_step(*this , power_dist , 0 , reestim);

        if (estimator != PENALIZED_LIKELIHOOD) {
          reestim->distribution_estimation(compound->distribution);
        }
        else {
          reestim->penalized_likelihood_estimation(compound->distribution , weight ,
                                                   penalty_type , penalty , outside);
        }
        break;
      }
      }

      // calcul de la loi composee estime et de la log-vraisemblance correspondante

      compound->computation(power_dist , nb_value , COMPOUND_THRESHOLD ,
                            sum_compute , dist_compute);
      previous_likelihood = likelihood;
      likelihood = compound->likelihood_computation(*this);

#     ifdef MESSAGE
      if ((i < 10) || ((i < 100) && (i % 10 == 0)) || ((i < 1000) && (i % 100 == 0)) || (i % 1000 == 0)) {
        os << STAT_label[STATL_ITERATION] << " " << i << "   "
           << STAT_label[STATL_LIKELIHOOD] << ": " << likelihood << "   "
           << STAT_label[STATL_SMOOTHNESS] << ": ";

        switch (type) {

        case 's' : {
          os << compound->sum_distribution->second_difference_norm_computation();
          if (estimator == PENALIZED_LIKELIHOOD) {
            os << "   cumul: " << compound->sum_distribution->cumul[compound->sum_distribution->nb_value - 1];
          }
          break;
        }

        case 'e' : {
          os << compound->distribution->second_difference_norm_computation();
          if (estimator == PENALIZED_LIKELIHOOD) {
            os << "   cumul: " << compound->distribution->cumul[compound->distribution->nb_value - 1];
          }
          break;
        }
        }

        os << endl;
      }
#     endif

    }
    while ((likelihood != D_INF) && (((nb_iter == I_DEFAULT) && (i < COMPOUND_NB_ITER) && 
             ((likelihood - previous_likelihood) / -likelihood > COMPOUND_LIKELIHOOD_DIFF)) ||
            ((nb_iter != I_DEFAULT) && (i < nb_iter))));

    if (likelihood != D_INF) {

#     ifdef MESSAGE
      os << "\n" << i << " " << STAT_label[STATL_ITERATIONS] << "   "
           << STAT_label[STATL_LIKELIHOOD] << ": " << likelihood << "   "
           << STAT_label[STATL_SMOOTHNESS] << ": ";

      switch (type) {

      case 's' : {
        os << compound->sum_distribution->second_difference_norm_computation();
        if (estimator == PENALIZED_LIKELIHOOD) {
          os << "   cumul: " << compound->sum_distribution->cumul[compound->sum_distribution->nb_value - 1];
        }
        break;
      }

      case 'e' : {
        os << compound->distribution->second_difference_norm_computation();
        if (estimator == PENALIZED_LIKELIHOOD) {
          os << "   cumul: " << compound->distribution->cumul[compound->distribution->nb_value - 1];
        }
        break;
      }
      }

      os << endl;
#     endif

      if (estimator == PARAMETRIC_REGULARIZATION) {
        likelihood = D_INF;
        nb_likelihood_decrease = 0;

        i = 0;
        do {
          i++;

          // calcul des quantites de reestimation

          switch (type) {

          case 's' : {
            compound->expectation_step(*this , power_dist , sum_reestim , 0);
            compound_histo->sum_histogram->update(sum_reestim , (int)(sum_reestim->nb_element *
                                                                      MAX(sqrt(sum_reestim->variance) , 1.) * COMPOUND_COEFF));
            hlikelihood = compound_histo->sum_histogram->Reestimation<int>::type_parametric_estimation(compound->sum_distribution ,
                                                                                                       MIN(sum_dist.offset , 1) , true);
            break;
          }

          case 'e' : {
            compound->expectation_step(*this , power_dist , 0 , reestim);
            compound_histo->histogram->update(reestim , (int)(reestim->nb_element *
                                                              MAX(sqrt(reestim->variance) , 1.) * COMPOUND_COEFF));
            hlikelihood = compound_histo->histogram->Reestimation<int>::type_parametric_estimation(compound->distribution , MIN(dist.offset , 1) ,
                                                                                                   true , COMPOUND_THRESHOLD);
            break;
          }
          }

          if (hlikelihood == D_INF) {
            likelihood = D_INF;
          }

          // calcul de la loi composee estime et de la log-vraisemblance correspondante

          else {
            compound->computation(power_dist , nb_value , COMPOUND_THRESHOLD ,
                                  sum_compute , dist_compute);
            previous_likelihood = likelihood;
            likelihood = compound->likelihood_computation(*this);

            if (likelihood < previous_likelihood) {
              nb_likelihood_decrease++;
            }
            else {
              nb_likelihood_decrease = 0;
            }

#           ifdef DEBUG
            if ((i < 10) || (i % 10 == 0)) {
              os << STAT_label[STATL_ITERATION] << " " << i << "   "
                 << STAT_label[STATL_LIKELIHOOD] << ": " << likelihood << "   "
                 << STAT_label[STATL_SMOOTHNESS] << ": ";

              switch (type) {
              case 's' :
                os << compound->sum_distribution->second_difference_norm_computation() << endl;
                break;
              case 'e' :
                os << compound->distribution->second_difference_norm_computation() << endl;
                break;
              }
            }
#           endif

          }
        }
        while ((likelihood != D_INF) && (i < COMPOUND_NB_ITER) &&
               (((likelihood - previous_likelihood) / -likelihood > COMPOUND_LIKELIHOOD_DIFF) ||
                (hlikelihood == D_INF) || (nb_likelihood_decrease == 1)));

#       ifdef MESSAGE
        if (likelihood != D_INF) {
          os << "\n" << i << " " << STAT_label[STATL_ITERATIONS] << "   "
             << STAT_label[STATL_LIKELIHOOD] << ": " << likelihood << "   "
             << STAT_label[STATL_SMOOTHNESS] << ": ";

          switch (type) {
          case 's' :
            os << compound->sum_distribution->second_difference_norm_computation() << endl;
            break;
          case 'e' :
            os << compound->distribution->second_difference_norm_computation() << endl;
            break;
          }
        }
#       endif

      }
    }

    if (likelihood != D_INF) {

      // mise a jour du nombre de parametres inconnus

      switch (type) {
      case 's' :
        compound->sum_distribution->nb_parameter_update();
        compound->nb_parameter = compound->sum_distribution->nb_parameter;
        break;
      case 'e' :
        compound->distribution->nb_parameter_update();
        compound->nb_parameter = compound->distribution->nb_parameter;
        break;
      }

      compound->expectation_step(*this , power_dist , sum_reestim , reestim);
      compound_histo->sum_histogram->update(sum_reestim , nb_element);
      compound_histo->histogram->update(reestim , (int)(nb_element * compound->sum_distribution->mean));
    }

    else {
      delete compound;
      compound = NULL;
      error.update(STAT_error[STATR_ESTIMATION_FAILURE]);
    }

    if (estimator == PENALIZED_LIKELIHOOD) {
      delete [] penalty;
    }

    for (i = MAX(sum_dist.offset - 1 , 1);i < sum_nb_value;i++) {
      delete power_dist[i];
    }
    delete [] power_dist;

    delete sum_reestim;
    delete reestim;
  }

  return compound;
}


/*--------------------------------------------------------------*
 *
 *  Estimation des parametres d'une loi composee par l'algorithme EM.
 *
 *  arguments : references sur un objet Format_error, stream, reference sur la loi connue,
 *              type de la loi inconnue ('s' : loi de la somme, 'e' : loi elementaire),
 *              borne inferieure minimum de la loi inconnue, type d'estimateur
 *              (vraisemblance, vraisemblance penalisee ou estimation d'une loi parametrique),
 *              nombre d'iterations, poids de la penalisation, type de penalisation,
 *              type de gestion des effets de bord (zero a l'exterieur du support ou
 *              prolongation de la loi).
 *
 *--------------------------------------------------------------*/

Compound* Histogram::compound_estimation(Format_error &error , ostream &os , const Parametric &known_dist ,
                                         char type , int min_inf_bound , int estimator , int nb_iter ,
                                         double weight , int penalty_type , int outside) const

{
  double proba;
  Parametric *unknown_dist;
  Compound *compound;


  error.init();

  if ((min_inf_bound != 0) && (min_inf_bound != 1)) {
    compound = NULL;
    error.update(STAT_error[STATR_MIN_INF_BOUND]);
  }

  else {
    proba = 1. / ((mean / known_dist.mean) - min_inf_bound + 1.);
    if (proba > 1. - COMPOUND_INIT_PROBABILITY) {
      proba = 1. - COMPOUND_INIT_PROBABILITY;
    }
    else if (proba < COMPOUND_INIT_PROBABILITY) {
      proba = COMPOUND_INIT_PROBABILITY;
    }

    switch (type) {
    case 's' :
      unknown_dist = new Parametric(NEGATIVE_BINOMIAL , min_inf_bound , I_DEFAULT , 1. , proba);
      break;
    case 'e' :
      unknown_dist = new Parametric(NEGATIVE_BINOMIAL , min_inf_bound , I_DEFAULT , 1. , proba ,
                                    COMPOUND_THRESHOLD);
      break;
    }

#   ifdef DEBUG
    unknown_dist->ascii_print(cout);
#   endif

    switch (type) {
    case 's' :
      compound = compound_estimation(error , os , *unknown_dist , known_dist , type ,
                                     estimator , nb_iter , weight , penalty_type , outside);
      break;
    case 'e' :
      compound = compound_estimation(error , os , known_dist , *unknown_dist , type ,
                                     estimator , nb_iter , weight , penalty_type , outside);
      break;
    }

    delete unknown_dist;
  }

  return compound;
}


/*--------------------------------------------------------------*
 *
 *  Simulation par une loi composee.
 *
 *  arguments : reference sur un objet Format_error, effectif.
 *
 *--------------------------------------------------------------*/

Compound_data* Compound::simulation(Format_error &error , int nb_element) const

{
  register int i , j;
  int nb_dist , sum , value;
  Compound_data *compound_histo;


  error.init();

  if ((nb_element < 1) || (nb_element > DIST_NB_ELEMENT)) {
    compound_histo = NULL;
    error.update(STAT_error[STATR_SAMPLE_SIZE]);
  }

  else {

    // creation d'un objet Compound_data

    compound_histo = new Compound_data(*this);
    compound_histo->compound = new Compound(*this , false);

    for (i = 0;i < nb_element;i++) {

      // loi de la somme

      nb_dist = sum_distribution->simulation();
      (compound_histo->sum_histogram->frequency[nb_dist])++;

      sum = 0;
      for (j = 0;j < nb_dist;j++) {

        // loi elementaire

        value = distribution->simulation();
        sum += value;
        (compound_histo->histogram->frequency[value])++;
      }

      // loi resultante

      (compound_histo->frequency[sum])++;
    }

    // extraction des caracteristiques des histogrammes

    compound_histo->nb_value_computation();
    compound_histo->offset_computation();
    compound_histo->nb_element = nb_element;
    compound_histo->max_computation();
    compound_histo->mean_computation();
    compound_histo->variance_computation();

    compound_histo->sum_histogram->nb_value_computation();
    compound_histo->sum_histogram->offset_computation();
    compound_histo->sum_histogram->nb_element_computation();
    compound_histo->sum_histogram->max_computation();
    compound_histo->sum_histogram->mean_computation();
    compound_histo->sum_histogram->variance_computation();

    compound_histo->histogram->nb_value_computation();
    compound_histo->histogram->offset_computation();
    compound_histo->histogram->nb_element_computation();
    compound_histo->histogram->max_computation();
    compound_histo->histogram->mean_computation();
    compound_histo->histogram->variance_computation();
  }

  return compound_histo;
}
