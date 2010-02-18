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



#include "tool/rw_tokenizer.h"
#include "tool/rw_cstring.h"
#include "tool/rw_locale.h"
#include "stat_tool/stat_tools.h"
#include "stat_tool/curves.h"
#include "stat_tool/markovian.h"
#include "stat_tool/stat_label.h"
#include "sequences.h"
#include "sequence_label.h"

using namespace std;



/*--------------------------------------------------------------*
 *
 *  creation des lois d'un objet NonparametricSequenceProcess
 *  a l'exception des lois d'observation.
 *
 *  arguments : loi des longueurs des sequences, homogeneite des etats,
 *              flag sur les lois de comptage.
 *
 *--------------------------------------------------------------*/

void NonparametricSequenceProcess::create_characteristic(const Distribution &ilength ,
                                                         bool *homogeneity ,
                                                         bool counting_flag)

{
  bool homogeneous = true;
  register int i;
  int max_length = ilength.nb_value - 1;


  if (length) {
    delete length;
  }
  length = new Distribution(ilength);

  if (index_value) {
    delete index_value;
  }
  index_value = new Curves(nb_value , max_length , false , false);

  if (!no_occurrence) {
    no_occurrence = new double[nb_value];
  }

  for (i = 0;i < nb_value;i++) {
    no_occurrence[i] = 0.;
  }

  if (first_occurrence) {
    for (i = 0;i < nb_value;i++) {
      delete first_occurrence[i];
    }
  }
  else {
    first_occurrence = new Distribution*[nb_value];
  }

  for (i = 0;i < nb_value;i++) {
    first_occurrence[i] = new Distribution(NB_VALUE);
  }

  if (!absorption) {
    absorption = new double[nb_value];
  }

  for (i = 0;i < nb_value;i++) {
    absorption[i] = 0.;
  }

  if (sojourn_time) {
    for (i = 0;i < nb_value;i++) {
      delete sojourn_time[i];
    }
  }
  else {
    sojourn_time = new DiscreteParametric*[nb_value];
  }

  for (i = 0;i < nb_value;i++) {
    if (homogeneity[i]) {
      sojourn_time[i] = new DiscreteParametric(NB_VALUE);
    }
    else {
      sojourn_time[i] = NULL;
      homogeneous = false;
    }
  }

  if (homogeneous) {
    if (!leave) {
      leave = new double[nb_value];
    }

    for (i = 0;i < nb_value;i++) {
      leave[i] = 0.;
    }

    if (recurrence_time) {
      for (i = 0;i < nb_value;i++) {
        delete recurrence_time[i];
      }
    }
    else {
      recurrence_time = new Distribution*[nb_value];
    }

    for (i = 0;i < nb_value;i++) {
      recurrence_time[i] = new Distribution(NB_VALUE);
    }
  }

  if (counting_flag) {
    if (nb_run) {
      for (i = 0;i < nb_value;i++) {
        delete nb_run[i];
      }
    }
    else {
      nb_run = new Distribution*[nb_value];
    }

    for (i = 0;i < nb_value;i++) {
      nb_run[i] = new Distribution((max_length % 2 == 0 ?
                                    max_length / 2 : max_length / 2 + 1) + 1);
    }

    if (nb_occurrence) {
      for (i = 0;i < nb_value;i++) {
        delete nb_occurrence[i];
      }
    }
    else {
      nb_occurrence = new Distribution*[nb_value];
    }

    for (i = 0;i < nb_value;i++) {
      nb_occurrence[i] = new Distribution(max_length + 1);
    }
  }
}


/*--------------------------------------------------------------*
 *
 *  creation des lois d'un objet NonparametricSequenceProcess
 *  a l'exception des lois d'observation.
 *
 *  arguments : loi des longueurs des sequences, flags sur les lois
 *              de temps de sejour et sur les lois de comptage.
 *
 *--------------------------------------------------------------*/

void NonparametricSequenceProcess::create_characteristic(const Distribution &ilength ,
                                                         bool sojourn_time_flag , bool counting_flag)

{
  register int i;
  int max_length = ilength.nb_value - 1;


  if (length) {
    delete length;
  }
  length = new Distribution(ilength);

  if (index_value) {
    delete index_value;
  }
  index_value = new Curves(nb_value , max_length , false , false);

  if (!no_occurrence) {
    no_occurrence = new double[nb_value];
  }

  for (i = 0;i < nb_value;i++) {
    no_occurrence[i] = 0.;
  }

  if (first_occurrence) {
    for (i = 0;i < nb_value;i++) {
      delete first_occurrence[i];
    }
  }
  else {
    first_occurrence = new Distribution*[nb_value];
  }

  for (i = 0;i < nb_value;i++) {
    first_occurrence[i] = new Distribution(NB_VALUE);
  }

  if (!leave) {
    leave = new double[nb_value];
  }

  for (i = 0;i < nb_value;i++) {
    leave[i] = 0.;
  }

  if (recurrence_time) {
    for (i = 0;i < nb_value;i++) {
      delete recurrence_time[i];
    }
  }
  else {
    recurrence_time = new Distribution*[nb_value];
  }

  for (i = 0;i < nb_value;i++) {
    recurrence_time[i] = new Distribution(NB_VALUE);
  }

  if (sojourn_time_flag) {
    if (!absorption) {
      absorption = new double[nb_value];
    }

    for (i = 0;i < nb_value;i++) {
      absorption[i] = 0.;
    }

    if (sojourn_time) {
      for (i = 0;i < nb_value;i++) {
        delete sojourn_time[i];
      }
    }
    else {
      sojourn_time = new DiscreteParametric*[nb_value];
    }

    for (i = 0;i < nb_value;i++) {
      sojourn_time[i] = new DiscreteParametric(NB_VALUE);
    }
  }

  if (counting_flag) {
    if (nb_run) {
      for (i = 0;i < nb_value;i++) {
        delete nb_run[i];
      }
    }
    else {
      nb_run = new Distribution*[nb_value];
    }

    for (i = 0;i < nb_value;i++) {
      nb_run[i] = new Distribution((max_length % 2 == 0 ?
                                    max_length / 2 : max_length / 2 + 1) + 1);
    }

    if (nb_occurrence) {
      for (i = 0;i < nb_value;i++) {
        delete nb_occurrence[i];
      }
    }
    else {
      nb_occurrence = new Distribution*[nb_value];
    }

    for (i = 0;i < nb_value;i++) {
      nb_occurrence[i] = new Distribution(max_length + 1);
    }
  }
}


/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe NonparametricSequenceProcess.
 *
 *  arguments : nombre d'etats, nombre de valeurs,
 *              flag sur les lois d'observation.
 *
 *--------------------------------------------------------------*/

NonparametricSequenceProcess::NonparametricSequenceProcess(int inb_state , int inb_value ,
                                                           int observation_flag)
:NonparametricProcess(inb_state , inb_value , observation_flag)

{
  length = NULL;
  index_value = NULL;
  no_occurrence = NULL;
  first_occurrence = NULL;
  leave = NULL;
  recurrence_time = NULL;
  absorption = NULL;
  sojourn_time = NULL;
  nb_run = NULL;
  nb_occurrence = NULL;
}


/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe NonparametricSequenceProcess.
 *
 *  arguments : nombre d'etats, lois d'occupation des etats.
 *
 *--------------------------------------------------------------*/

NonparametricSequenceProcess::NonparametricSequenceProcess(int inb_state ,
                                                           DiscreteParametric **occupancy)

{
  register int i;


  nb_state = inb_state;
  nb_value = inb_state;

  observation = NULL;

  length = NULL;
  index_value = NULL;
  no_occurrence = NULL;
  first_occurrence = NULL;
  leave = NULL;
  recurrence_time = NULL;

  absorption = new double[nb_state];
  sojourn_time = new DiscreteParametric*[nb_state];
  for (i = 0;i < nb_state;i++) {
    if (occupancy[i]) {
      sojourn_time[i] = new DiscreteParametric(*occupancy[i]);
    }
    else {
      sojourn_time[i] = NULL;
    }
  }

  nb_run = NULL;
  nb_occurrence = NULL;
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet NonparametricSequenceProcess a partir
 *  d'un objet NonparametricProcess.
 *
 *  argument : reference sur un objet NonparametricProcess.
 *
 *--------------------------------------------------------------*/

NonparametricSequenceProcess::NonparametricSequenceProcess(const NonparametricProcess &process)
:NonparametricProcess(process)

{
  length = NULL;
  index_value = NULL;
  no_occurrence = NULL;
  first_occurrence = NULL;
  leave = NULL;
  recurrence_time = NULL;
  absorption = NULL;
  sojourn_time = NULL;
  nb_run = NULL;
  nb_occurrence = NULL;
}


/*--------------------------------------------------------------*
 *
 *  Copie d'un objet NonparametricSequenceProcess.
 *
 *  arguments : reference sur un objet NonparametricSequenceProcess,
 *              flag copie des lois caracteristiques.
 *
 *--------------------------------------------------------------*/

void NonparametricSequenceProcess::copy(const NonparametricSequenceProcess &process ,
                                        bool characteristic_flag)

{
  if (characteristic_flag) {
    register int i;


    if (process.length) {
      length = new Distribution(*(process.length));
    }
    else {
      length = NULL;
    }

    if (process.index_value) {
      index_value = new Curves(*(process.index_value));
    }
    else {
      index_value = NULL;
    }

    if (process.no_occurrence) {
      no_occurrence = new double[nb_value];
      for (i = 0;i < nb_value;i++) {
        no_occurrence[i] = process.no_occurrence[i];
      }
    }
    else {
      no_occurrence = NULL;
    }

    if (process.first_occurrence) {
      first_occurrence = new Distribution*[nb_value];
      for (i = 0;i < nb_value;i++) {
        first_occurrence[i] = new Distribution(*(process.first_occurrence[i]));
      }
    }
    else {
      first_occurrence = NULL;
    }

    if (process.leave) {
      leave = new double[nb_value];
      for (i = 0;i < nb_value;i++) {
        leave[i] = process.leave[i];
      }
    }
    else {
      leave = NULL;
    }

    if (process.recurrence_time) {
      recurrence_time = new Distribution*[nb_value];
      for (i = 0;i < nb_value;i++) {
        if (process.recurrence_time[i]) {
          recurrence_time[i] = new Distribution(*(process.recurrence_time[i]));
        }
        else {
          recurrence_time[i] = NULL;
        }
      }
    }
    else {
      recurrence_time = NULL;
    }

    if (process.absorption) {
      absorption = new double[nb_value];
      for (i = 0;i < nb_value;i++) {
        absorption[i] = process.absorption[i];
      }
    }
    else {
      absorption = NULL;
    }

    if (process.sojourn_time) {
      sojourn_time = new DiscreteParametric*[nb_value];
      for (i = 0;i < nb_value;i++) {
        if (process.sojourn_time[i]) {
          sojourn_time[i] = new DiscreteParametric(*(process.sojourn_time[i]));
        }
        else {
          sojourn_time[i] = NULL;
        }
      }
    }
    else {
      sojourn_time = NULL;
    }

    if (process.nb_run) {
      nb_run = new Distribution*[nb_value];
      for (i = 0;i < nb_value;i++) {
        nb_run[i] = new Distribution(*(process.nb_run[i]));
      }
    }
    else {
      nb_run = NULL;
    }

    if (process.nb_occurrence) {
      nb_occurrence = new Distribution*[nb_value];
      for (i = 0;i < nb_value;i++) {
        nb_occurrence[i] = new Distribution(*(process.nb_occurrence[i]));
      }
    }
    else {
      nb_occurrence = NULL;
    }
  }

  else {
    length = NULL;
    index_value = NULL;
    no_occurrence = NULL;
    first_occurrence = NULL;
    leave = NULL;
    recurrence_time = NULL;
    absorption = NULL;
    sojourn_time = NULL;
    nb_run = NULL;
    nb_occurrence = NULL;
  }
}


/*--------------------------------------------------------------*
 *
 *  Copie des lois d'occupation des etats.
 *
 *  arguments : reference sur un objet NonparametricSequenceProcess,
 *              nombre de valeurs allouees pour les lois d'occupation des etats.
 *
 *--------------------------------------------------------------*/

void NonparametricSequenceProcess::init_occupancy(const NonparametricSequenceProcess &process ,
                                                  int occupancy_nb_value)

{
  register int i;


  nb_state = process.nb_state;
  nb_value = process.nb_value;

  observation = NULL;

  length = NULL;
  index_value = NULL;
  no_occurrence = NULL;
  first_occurrence = NULL;
  leave = NULL;
  recurrence_time = NULL;

  absorption = new double[nb_value];
  sojourn_time = new DiscreteParametric*[nb_value];
  for (i = 0;i < nb_value;i++) {
    absorption[i] = process.absorption[i];
    if ((process.sojourn_time[i]) && (process.sojourn_time[i]->ident != NONPARAMETRIC)) {
      sojourn_time[i] = new DiscreteParametric(*(process.sojourn_time[i]) ,
                                               'c' , occupancy_nb_value);
    }
    else {
      sojourn_time[i] = NULL;
    }
  }

  nb_run = NULL;
  nb_occurrence = NULL;
}


/*--------------------------------------------------------------*
 *
 *  Constructeur par copie de la classe NonparametricSequenceProcess.
 *
 *  arguments : reference sur un objet NonparametricSequenceProcess,
 *              type de manipulation ('c' : copy, 's' : state, 'o' :occupancy),
 *              parametre (flag sur le calcul des lois caracteristiques /
 *              nombre de valeurs allouees pour les lois d'occupation des etats /
 *              etat de reference).
 *
 *--------------------------------------------------------------*/

NonparametricSequenceProcess::NonparametricSequenceProcess(const NonparametricSequenceProcess &process ,
                                                           char manip , int param)

{
  switch (manip) {

  case 'c' : {
    NonparametricProcess::copy(process);
    copy(process , param);
    break;
  }

  case 'o' : {
    init_occupancy(process , param);
    break;
  }

  case 's' : {
    add_state(process , param);

    length = NULL;
    index_value = NULL;
    no_occurrence = NULL;
    first_occurrence = NULL;
    leave = NULL;
    recurrence_time = NULL;
    absorption = NULL;
    sojourn_time = NULL;
    nb_run = NULL;
    nb_occurrence = NULL;
    break;
  }
  }
}


/*--------------------------------------------------------------*
 *
 *  Destruction des champs d'un objet NonparametricSequenceProcess.
 *
 *--------------------------------------------------------------*/

void NonparametricSequenceProcess::remove()

{
  register int i;


  if (length) {
    delete length;

    length = NULL;
  }

  if (index_value) {
    delete index_value;

    index_value = NULL;
  }

  if (no_occurrence) {
    delete [] no_occurrence;

    no_occurrence = NULL;
  }

  if (first_occurrence) {
    for (i = 0;i < nb_value;i++) {
      delete first_occurrence[i];
    }
    delete [] first_occurrence;

    first_occurrence = NULL;
  }

  if (leave) {
    delete [] leave;

    leave = NULL;
  }

  if (recurrence_time) {
    for (i = 0;i < nb_value;i++) {
      delete recurrence_time[i];
    }
    delete [] recurrence_time;

    recurrence_time = NULL;
  }

  if (absorption) {
    delete [] absorption;

    absorption = NULL;
  }

  if (sojourn_time) {
    for (i = 0;i < nb_value;i++) {
      delete sojourn_time[i];
    }
    delete [] sojourn_time;

    sojourn_time = NULL;
  }

  if (nb_run) {
    for (i = 0;i < nb_value;i++) {
      delete nb_run[i];
    }
    delete [] nb_run;

    nb_run = NULL;
  }

  if (nb_occurrence) {
    for (i = 0;i < nb_value;i++) {
      delete nb_occurrence[i];
    }
    delete [] nb_occurrence;

    nb_occurrence = NULL;
  }
}


/*--------------------------------------------------------------*
 *
 *  Destructeur de la classe NonparametricSequenceProcess.
 *
 *--------------------------------------------------------------*/

NonparametricSequenceProcess::~NonparametricSequenceProcess()

{
  remove();
}


/*--------------------------------------------------------------*
 *
 *  Operateur d'assignement de la classe NonparametricSequenceProcess.
 *
 *  argument : reference sur un objet NonparametricSequenceProcess.
 *
 *--------------------------------------------------------------*/

NonparametricSequenceProcess& NonparametricSequenceProcess::operator=(const NonparametricSequenceProcess &process)

{
  if (&process != this) {
    remove();
    NonparametricProcess::remove();

    NonparametricProcess::copy(process);
    copy(process);
  }

  return *this;
}


/*--------------------------------------------------------------*
 *
 *  test modele cache
 *
 *  arguments : nombre de processus d'observation, pointeurs sur ces processus.
 *
 *--------------------------------------------------------------*/

bool test_hidden(int nb_output_process , NonparametricSequenceProcess **process)

{
  bool hidden = false;
  register int i;


  for (i = 1;i <= nb_output_process;i++) {
    if (process[i]) {
      hidden = process[i]->test_hidden();
      if (hidden) {
        break;
      }
    }

    else {
      hidden = true;
      break;
    }
  }

  return hidden;
}


/*--------------------------------------------------------------*
 *
 *  Analyse du format des lois d'occupation des etats.
 *
 *  arguments : reference sur un objet StatError, stream,
 *              reference sur l'indice de la ligne lue et sur un objet Chain,
 *              seuil sur la fonction de repartition.
 *
 *--------------------------------------------------------------*/

NonparametricSequenceProcess* occupancy_parsing(StatError &error , ifstream &in_file ,
                                                int &line , const Chain &chain ,
                                                double cumul_threshold)

{
  RWLocaleSnapshot locale("en");
  RWCString buffer , token;
  size_t position;
  bool status = true , lstatus;
  register int i , j;
  long index;
  DiscreteParametric **dist;
  NonparametricSequenceProcess *process;


  process = NULL;

  dist = new DiscreteParametric*[chain.nb_state];
  for (i = 0;i < chain.nb_state;i++) {
    dist[i] = NULL;
  }

  for (i = 0;i < chain.nb_state;i++) {
    if (chain.transition[i][i] == 0.) {
      while (buffer.readLine(in_file , false)) {
        line++;

#       ifdef DEBUG
        cout << line << "  " << buffer << endl;
#       endif

        position = buffer.first('#');
        if (position != RW_NPOS) {
          buffer.remove(position);
        }
        j = 0;

        RWCTokenizer next(buffer);

        while (!((token = next()).isNull())) {
          switch (j) {

          // test mot cle STATE

          case 0 : {
            if (token != STAT_word[STATW_STATE]) {
              status = false;
              error.correction_update(STAT_parsing[STATP_KEY_WORD] , STAT_word[STATW_STATE] , line , j + 1);
            }
            break;
          }

          // test indice de l'etat

          case 1 : {
            lstatus = locale.stringToNum(token , &index);
            if ((lstatus) && (index != i)) {
              lstatus = false;
            }

            if (!lstatus) {
              status = false;
              error.correction_update(STAT_parsing[STATP_STATE_INDEX] , i , line , j + 1);
            }
            break;
          }

          // test mot cle OCCUPANCY_DISTRIBUTION

          case 2 : {
            if (token != SEQ_word[SEQW_OCCUPANCY_DISTRIBUTION]) {
              status = false;
              error.correction_update(STAT_parsing[STATP_KEY_WORD] , SEQ_word[SEQW_OCCUPANCY_DISTRIBUTION] , line , j + 1);
            }
            break;
          }
          }

          j++;
        }

        if (j > 0) {
          if (j != 3) {
            status = false;
            error.update(STAT_parsing[STATP_FORMAT] , line);
          }

          dist[i] = discrete_parametric_parsing(error , in_file , line , UNIFORM ,
                                                cumul_threshold , 1);
          if (!dist[i]) {
            status = false;
          }

          else if (dist[i]->mean == 1.) {
            delete dist[i];
            dist[i] = NULL;
          }
          break;
        }
      }
    }
  }

  if (status) {
    process = new NonparametricSequenceProcess(chain.nb_state , dist);
  }

  for (i = 0;i < chain.nb_state;i++) {
    delete dist[i];
  }
  delete [] dist;

  return process;
}
