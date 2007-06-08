/* -*-c++-*-
 *  ----------------------------------------------------------------------------
 *
 *       AMAPmod: Exploring and Modeling Plant Architecture
 *
 *       Copyright 1995-2002 UMR Cirad/Inra Modelisation des Plantes
 *
 *       File author(s): Y. Guedon (yann.guedon@cirad.fr)
 *
 *       $Source$
 *       $Id$
 *
 *       Forum for AMAPmod developers: amldevlp@cirad.fr
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
 *  creation des lois d'un objet Nonparametric_sequence_process
 *  a l'exception des lois d'observation.
 *
 *  arguments : loi des longueurs des sequences, homogeneite des etats,
 *              flag sur les lois de comptage.
 *
 *--------------------------------------------------------------*/

void Nonparametric_sequence_process::create_characteristic(const Distribution &ilength ,
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
    sojourn_time = new Parametric*[nb_value];
  }

  for (i = 0;i < nb_value;i++) {
    if (homogeneity[i]) {
      sojourn_time[i] = new Parametric(NB_VALUE);
    }
    else {
      sojourn_time[i] = 0;
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
 *  creation des lois d'un objet Nonparametric_sequence_process
 *  a l'exception des lois d'observation.
 *
 *  arguments : loi des longueurs des sequences, flags sur les lois
 *              de temps de sejour et sur les lois de comptage.
 *
 *--------------------------------------------------------------*/

void Nonparametric_sequence_process::create_characteristic(const Distribution &ilength ,
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
      sojourn_time = new Parametric*[nb_value];
    }

    for (i = 0;i < nb_value;i++) {
      sojourn_time[i] = new Parametric(NB_VALUE);
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
 *  Constructeur de la classe Nonparametric_sequence_process.
 *
 *  arguments : nombre d'etats, nombre de valeurs,
 *              flag sur les lois d'observation.
 *
 *--------------------------------------------------------------*/

Nonparametric_sequence_process::Nonparametric_sequence_process(int inb_state , int inb_value ,
                                                               int observation_flag)
:Nonparametric_process(inb_state , inb_value , observation_flag)

{
  length = 0;
  index_value = 0;
  no_occurrence = 0;
  first_occurrence = 0;
  leave = 0;
  recurrence_time = 0;
  absorption = 0;
  sojourn_time = 0;
  nb_run = 0;
  nb_occurrence = 0;
}


/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe Nonparametric_sequence_process.
 *
 *  arguments : nombre d'etats, lois d'occupation des etats.
 *
 *--------------------------------------------------------------*/

Nonparametric_sequence_process::Nonparametric_sequence_process(int inb_state , Parametric **occupancy)

{
  register int i;


  nb_state = inb_state;
  nb_value = inb_state;

  observation = 0;

  length = 0;
  index_value = 0;
  no_occurrence = 0;
  first_occurrence = 0;
  leave = 0;
  recurrence_time = 0;

  absorption = new double[nb_state];
  sojourn_time = new Parametric*[nb_state];
  for (i = 0;i < nb_state;i++) {
    if (occupancy[i]) {
      sojourn_time[i] = new Parametric(*occupancy[i]);
    }
    else {
      sojourn_time[i] = 0;
    }
  }

  nb_run = 0;
  nb_occurrence = 0;
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet Nonparametric_sequence_process a partir
 *  d'un objet Nonparametric_process.
 *
 *  argument : reference sur un objet Nonparametric_process.
 *
 *--------------------------------------------------------------*/

Nonparametric_sequence_process::Nonparametric_sequence_process(const Nonparametric_process &process)
:Nonparametric_process(process)

{
  length = 0;
  index_value = 0;
  no_occurrence = 0;
  first_occurrence = 0;
  leave = 0;
  recurrence_time = 0;
  absorption = 0;
  sojourn_time = 0;
  nb_run = 0;
  nb_occurrence = 0;
}


/*--------------------------------------------------------------*
 *
 *  Copie d'un objet Nonparametric_sequence_process.
 *
 *  arguments : reference sur un objet Nonparametric_sequence_process,
 *              flag copie des lois caracteristiques.
 *
 *--------------------------------------------------------------*/

void Nonparametric_sequence_process::copy(const Nonparametric_sequence_process &process ,
                                          bool characteristic_flag)

{
  if (characteristic_flag) {
    register int i;


    if (process.length) {
      length = new Distribution(*(process.length));
    }
    else {
      length = 0;
    }

    if (process.index_value) {
      index_value = new Curves(*(process.index_value));
    }
    else {
      index_value = 0;
    }

    if (process.no_occurrence) {
      no_occurrence = new double[nb_value];
      for (i = 0;i < nb_value;i++) {
        no_occurrence[i] = process.no_occurrence[i];
      }
    }
    else {
      no_occurrence = 0;
    }

    if (process.first_occurrence) {
      first_occurrence = new Distribution*[nb_value];
      for (i = 0;i < nb_value;i++) {
        first_occurrence[i] = new Distribution(*(process.first_occurrence[i]));
      }
    }
    else {
      first_occurrence = 0;
    }

    if (process.leave) {
      leave = new double[nb_value];
      for (i = 0;i < nb_value;i++) {
        leave[i] = process.leave[i];
      }
    }
    else {
      leave = 0;
    }

    if (process.recurrence_time) {
      recurrence_time = new Distribution*[nb_value];
      for (i = 0;i < nb_value;i++) {
        if (process.recurrence_time[i]) {
          recurrence_time[i] = new Distribution(*(process.recurrence_time[i]));
        }
        else {
          recurrence_time[i] = 0;
        }
      }
    }
    else {
      recurrence_time = 0;
    }

    if (process.absorption) {
      absorption = new double[nb_value];
      for (i = 0;i < nb_value;i++) {
        absorption[i] = process.absorption[i];
      }
    }
    else {
      absorption = 0;
    }

    if (process.sojourn_time) {
      sojourn_time = new Parametric*[nb_value];
      for (i = 0;i < nb_value;i++) {
        if (process.sojourn_time[i]) {
          sojourn_time[i] = new Parametric(*(process.sojourn_time[i]));
        }
        else {
          sojourn_time[i] = 0;
        }
      }
    }
    else {
      sojourn_time = 0;
    }

    if (process.nb_run) {
      nb_run = new Distribution*[nb_value];
      for (i = 0;i < nb_value;i++) {
        nb_run[i] = new Distribution(*(process.nb_run[i]));
      }
    }
    else {
      nb_run = 0;
    }

    if (process.nb_occurrence) {
      nb_occurrence = new Distribution*[nb_value];
      for (i = 0;i < nb_value;i++) {
        nb_occurrence[i] = new Distribution(*(process.nb_occurrence[i]));
      }
    }
    else {
      nb_occurrence = 0;
    }
  }

  else {
    length = 0;
    index_value = 0;
    no_occurrence = 0;
    first_occurrence = 0;
    leave = 0;
    recurrence_time = 0;
    absorption = 0;
    sojourn_time = 0;
    nb_run = 0;
    nb_occurrence = 0;
  }
}


/*--------------------------------------------------------------*
 *
 *  Copie des lois d'occupation des etats.
 *
 *  arguments : reference sur un objet Nonparametric_sequence_process,
 *              nombre de valeurs allouees pour les lois d'occupation des etats.
 *
 *--------------------------------------------------------------*/

void Nonparametric_sequence_process::init_occupancy(const Nonparametric_sequence_process &process ,
                                                    int occupancy_nb_value)

{
  register int i;


  nb_state = process.nb_state;
  nb_value = process.nb_value;

  observation = 0;

  length = 0;
  index_value = 0;
  no_occurrence = 0;
  first_occurrence = 0;
  leave = 0;
  recurrence_time = 0;

  absorption = new double[nb_value];
  sojourn_time = new Parametric*[nb_value];
  for (i = 0;i < nb_value;i++) {
    absorption[i] = process.absorption[i];
    if ((process.sojourn_time[i]) && (process.sojourn_time[i]->ident != NONPARAMETRIC)) {
      sojourn_time[i] = new Parametric(*(process.sojourn_time[i]) , 'c' , occupancy_nb_value);
    }
    else {
      sojourn_time[i] = 0;
    }
  }

  nb_run = 0;
  nb_occurrence = 0;
}


/*--------------------------------------------------------------*
 *
 *  Constructeur par copie de la classe Nonparametric_sequence_process.
 *
 *  arguments : reference sur un objet Nonparametric_sequence_process,
 *              type de manipulation ('c' : copy, 's' : state, 'o' :occupancy),
 *              parametre (flag sur le calcul des lois caracteristiques /
 *              nombre de valeurs allouees pour les lois d'occupation des etats /
 *              etat de reference).
 *
 *--------------------------------------------------------------*/

Nonparametric_sequence_process::Nonparametric_sequence_process(const Nonparametric_sequence_process &process ,
                                                               char manip , int param)

{
  switch (manip) {

  case 'c' : {
    Nonparametric_process::copy(process);
    copy(process , param);
    break;
  }

  case 'o' : {
    init_occupancy(process , param);
    break;
  }

  case 's' : {
    add_state(process , param);

    length = 0;
    index_value = 0;
    no_occurrence = 0;
    first_occurrence = 0;
    leave = 0;
    recurrence_time = 0;
    absorption = 0;
    sojourn_time = 0;
    nb_run = 0;
    nb_occurrence = 0;
    break;
  }
  }
}


/*--------------------------------------------------------------*
 *
 *  Destruction des champs d'un objet Nonparametric_sequence_process.
 *
 *--------------------------------------------------------------*/

void Nonparametric_sequence_process::remove()

{
  register int i;


  if (length) {
    delete length;

    length = 0;
  }

  if (index_value) {
    delete index_value;

    index_value = 0;
  }

  if (no_occurrence) {
    delete [] no_occurrence;

    no_occurrence = 0;
  }

  if (first_occurrence) {
    for (i = 0;i < nb_value;i++) {
      delete first_occurrence[i];
    }
    delete [] first_occurrence;

    first_occurrence = 0;
  }

  if (leave) {
    delete [] leave;

    leave = 0;
  }

  if (recurrence_time) {
    for (i = 0;i < nb_value;i++) {
      delete recurrence_time[i];
    }
    delete [] recurrence_time;

    recurrence_time = 0;
  }

  if (absorption) {
    delete [] absorption;

    absorption = 0;
  }

  if (sojourn_time) {
    for (i = 0;i < nb_value;i++) {
      delete sojourn_time[i];
    }
    delete [] sojourn_time;

    sojourn_time = 0;
  }

  if (nb_run) {
    for (i = 0;i < nb_value;i++) {
      delete nb_run[i];
    }
    delete [] nb_run;

    nb_run = 0;
  }

  if (nb_occurrence) {
    for (i = 0;i < nb_value;i++) {
      delete nb_occurrence[i];
    }
    delete [] nb_occurrence;

    nb_occurrence = 0;
  }
}


/*--------------------------------------------------------------*
 *
 *  Destructeur de la classe Nonparametric_sequence_process.
 *
 *--------------------------------------------------------------*/

Nonparametric_sequence_process::~Nonparametric_sequence_process()

{
  remove();
}


/*--------------------------------------------------------------*
 *
 *  Operateur d'assignement de la classe Nonparametric_sequence_process.
 *
 *  argument : reference sur un objet Nonparametric_sequence_process.
 *
 *--------------------------------------------------------------*/

Nonparametric_sequence_process& Nonparametric_sequence_process::operator=(const Nonparametric_sequence_process &process)

{
  if (&process != this) {
    remove();
    Nonparametric_process::remove();

    Nonparametric_process::copy(process);
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

bool test_hidden(int nb_output_process , Nonparametric_sequence_process **process)

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
 *  arguments : reference sur un objet Format_error, stream,
 *              reference sur l'indice de la ligne lue et sur un objet Chain,
 *              seuil sur la fonction de repartition.
 *
 *--------------------------------------------------------------*/

Nonparametric_sequence_process* occupancy_parsing(Format_error &error , ifstream &in_file ,
                                                  int &line , const Chain &chain ,
                                                  double cumul_threshold)

{
  RWLocaleSnapshot locale("en");
  RWCString buffer , token;
  size_t position;
  bool status = true , lstatus;
  register int i , j;
  long index;
  Parametric **dist;
  Nonparametric_sequence_process *process;


  process = 0;

  dist = new Parametric*[chain.nb_state];
  for (i = 0;i < chain.nb_state;i++) {
    dist[i] = 0;
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

          dist[i] = parametric_parsing(error , in_file , line , UNIFORM ,
                                       cumul_threshold , 1);
          if (!dist[i]) {
            status = false;
          }

          else if (dist[i]->mean == 1.) {
            delete dist[i];
            dist[i] = 0;
          }
          break;
        }
      }
    }
  }

  if (status) {
    process = new Nonparametric_sequence_process(chain.nb_state , dist);
  }

  for (i = 0;i < chain.nb_state;i++) {
    delete dist[i];
  }
  delete [] dist;

  return process;
}
