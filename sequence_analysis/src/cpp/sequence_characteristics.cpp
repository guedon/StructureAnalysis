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



#include <sstream>
// #include <rw/vstream.h>
// #include <rw/rwfile.h>
#include "stat_tool/stat_tools.h"
#include "stat_tool/curves.h"
#include "stat_tool/markovian.h"
#include "stat_tool/stat_label.h"
#include "sequences.h"
#include "sequence_label.h"

using namespace std;


extern char* label(const char *file_name);



/*--------------------------------------------------------------*
 *
 *  Constructeur par defaut de la classe Sequence_characteristics.
 *
 *  argument : nombre de valeurs.
 *
 *--------------------------------------------------------------*/

Sequence_characteristics::Sequence_characteristics(int inb_value)

{
  nb_value = inb_value;

  index_value = 0;
  first_occurrence = 0;
  recurrence_time = 0;
  sojourn_time = 0;
  initial_run = 0;
  final_run = 0;
  nb_run = 0;
  nb_occurrence = 0;
}


/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe Sequence_characteristics avec ajout/suppression
 *  des histogrammes de temps de sejour initial.
 *
 *  arguments : reference sur un objet Sequence_characteristics,
 *              flag construction des histogrammes de temps de sejour initial.
 *
 *--------------------------------------------------------------*/

Sequence_characteristics::Sequence_characteristics(const Sequence_characteristics &characteristics ,
                                                   bool initial_run_flag)

{
  register int i;


  nb_value = characteristics.nb_value;

  index_value = new Curves(*(characteristics.index_value));

  if (characteristics.first_occurrence) {
    first_occurrence = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      first_occurrence[i] = new Histogram(*(characteristics.first_occurrence[i]));
    }
  }
  else {
    first_occurrence = 0;
  }

  recurrence_time = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    recurrence_time[i] = new Histogram(*(characteristics.recurrence_time[i]));
  }

  sojourn_time = new Histogram*[nb_value];

  if (initial_run_flag) {
    initial_run = new Histogram*[nb_value];
  }
  else {
    initial_run = 0;
  }

  final_run = new Histogram*[nb_value];

  for (i = 0;i < nb_value;i++) {
    if (((characteristics.initial_run) && (initial_run_flag)) ||
        ((!(characteristics.initial_run)) && (!initial_run_flag))) {
      sojourn_time[i] = new Histogram(*(characteristics.sojourn_time[i]));
      final_run[i] = new Histogram(*(characteristics.final_run[i]));
    }
    else {
      sojourn_time[i] = 0;
      final_run[i] = 0;
    }

    if ((characteristics.initial_run) && (initial_run_flag)) {
      initial_run[i] = new Histogram(*(characteristics.initial_run[i]));
    }
  }

  if (characteristics.nb_run) {
    nb_run = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      nb_run[i] = new Histogram(*(characteristics.nb_run[i]));
    }
  }
  else {
    nb_run = 0;
  }

  if (characteristics.nb_occurrence) {
    nb_occurrence = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      nb_occurrence[i] = new Histogram(*(characteristics.nb_occurrence[i]));
    }
  }
  else {
    nb_occurrence = 0;
  }
}


/*--------------------------------------------------------------*
 *
 *  Copie des caracteristiques d'un echantillon de sequences
 *  pour une variable donnee.
 *
 *  argument : reference sur un objet Sequence_characteristics.
 *
 *--------------------------------------------------------------*/

void Sequence_characteristics::copy(const Sequence_characteristics &characteristics)

{
  register int i;


  nb_value = characteristics.nb_value;

  index_value = new Curves(*(characteristics.index_value));

  if (characteristics.first_occurrence) {
    first_occurrence = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      first_occurrence[i] = new Histogram(*(characteristics.first_occurrence[i]));
    }
  }
  else {
    first_occurrence = 0;
  }

  recurrence_time = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    recurrence_time[i] = new Histogram(*(characteristics.recurrence_time[i]));
  }

  sojourn_time = new Histogram*[nb_value];

  if (characteristics.initial_run) {
    initial_run = new Histogram*[nb_value];
  }
  else {
    initial_run = 0;
  }

  final_run = new Histogram*[nb_value];

  for (i = 0;i < nb_value;i++) {
    sojourn_time[i] = new Histogram(*(characteristics.sojourn_time[i]));
    if (characteristics.initial_run) {
      initial_run[i] = new Histogram(*(characteristics.initial_run[i]));
    }
    final_run[i] = new Histogram(*(characteristics.final_run[i]));
  }

  if (characteristics.nb_run) {
    nb_run = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      nb_run[i] = new Histogram(*(characteristics.nb_run[i]));
    }
  }
  else {
    nb_run = 0;
  }

  if (characteristics.nb_occurrence) {
    nb_occurrence = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      nb_occurrence[i] = new Histogram(*(characteristics.nb_occurrence[i]));
    }
  }
  else {
    nb_occurrence = 0;
  }
}


/*--------------------------------------------------------------*
 *
 *  Copie des caracteristiques inchangees d'un echantillon de sequences
 *  pour une variable donnee dans le cas d'une inversion des sequences.
 *
 *  argument : reference sur un objet Sequence_characteristics.
 *
 *--------------------------------------------------------------*/

void Sequence_characteristics::reverse(const Sequence_characteristics &characteristics)

{
  register int i;


  nb_value = characteristics.nb_value;

  index_value = 0;
  first_occurrence = 0;

  recurrence_time = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    recurrence_time[i] = new Histogram(*(characteristics.recurrence_time[i]));
  }

  if (characteristics.initial_run) {
    sojourn_time = new Histogram*[nb_value];
    initial_run = new Histogram*[nb_value];
    final_run = new Histogram*[nb_value];

    for (i = 0;i < nb_value;i++) {
      sojourn_time[i] = new Histogram(*(characteristics.sojourn_time[i]));
      initial_run[i] = new Histogram(*(characteristics.final_run[i]));
      final_run[i] = new Histogram(*(characteristics.initial_run[i]));
    }
  }

  else {
    sojourn_time = 0;
    initial_run = 0;
    final_run = 0;
  }

  if (characteristics.nb_run) {
    nb_run = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      nb_run[i] = new Histogram(*(characteristics.nb_run[i]));
    }
  }
  else {
    nb_run = 0;
  }

  if (characteristics.nb_occurrence) {
    nb_occurrence = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      nb_occurrence[i] = new Histogram(*(characteristics.nb_occurrence[i]));
    }
  }
  else {
    nb_occurrence = 0;
  }
}


/*--------------------------------------------------------------*
 *
 *  Constructeur par copie de la classe Sequence_characteristics.
 *
 *  arguments : reference sur un objet Sequence_characteristics,
 *              type de transformation ('r' : inversion).
 *
 *--------------------------------------------------------------*/

Sequence_characteristics::Sequence_characteristics(const Sequence_characteristics &characteristics ,
                                                   char transform)

{
  switch (transform) {
  case 'r' :
    reverse(characteristics);
    break;
  default :
    copy(characteristics);
    break;
  }
}


/*--------------------------------------------------------------*
 *
 *  Destructeur des champs de la classe Sequence_characteristics.
 *
 *--------------------------------------------------------------*/

void Sequence_characteristics::remove()

{
  register int i;


  delete index_value;

  if (first_occurrence) {
    for (i = 0;i < nb_value;i++) {
      delete first_occurrence[i];
    }
    delete [] first_occurrence;
  }

  if (recurrence_time) {
    for (i = 0;i < nb_value;i++) {
      delete recurrence_time[i];
    }
    delete [] recurrence_time;
  }

  if (sojourn_time) {
    for (i = 0;i < nb_value;i++) {
      delete sojourn_time[i];
    }
    delete [] sojourn_time;
  }

  if (initial_run) {
    for (i = 0;i < nb_value;i++) {
      delete initial_run[i];
    }
    delete [] initial_run;
  }

  if (final_run) {
    for (i = 0;i < nb_value;i++) {
      delete final_run[i];
    }
    delete [] final_run;
  }

  if (nb_run) {
    for (i = 0;i < nb_value;i++) {
      delete nb_run[i];
    }
    delete [] nb_run;
  }

  if (nb_occurrence) {
    for (i = 0;i < nb_value;i++) {
      delete nb_occurrence[i];
    }
    delete [] nb_occurrence;
  }
}


/*--------------------------------------------------------------*
 *
 *  Destructeur de la classe Sequence_characteristics.
 *
 *--------------------------------------------------------------*/

Sequence_characteristics::~Sequence_characteristics()

{
  remove();
}


/*--------------------------------------------------------------*
 *
 *  Operateur d'assignement de la classe Sequence_characteristics.
 *
 *  argument : reference sur un objet Sequence_characteristics.
 *
 *--------------------------------------------------------------*/

Sequence_characteristics& Sequence_characteristics::operator=(const Sequence_characteristics &characteristics)

{
  if (&characteristics != this) {
    remove();
    copy(characteristics);
  }

  return *this;
}


/*--------------------------------------------------------------*
 *
 *  Creation des histogrammes de temps de sejour dans une valeur
 *  pour une variable donnee.
 *
 *  arguments : longueur maximum des sequences, flag sur la creation
 *              des histogrammes de temps de sejour initial.
 *
 *--------------------------------------------------------------*/

void Sequence_characteristics::create_sojourn_time_histogram(int max_length , int initial_run_flag)

{
  register int i;


  sojourn_time = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    sojourn_time[i] = new Histogram(max_length + 1);
  }

  if (initial_run_flag) {
    initial_run = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      initial_run[i] = new Histogram(max_length + 1);
    }
  }

  final_run = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    final_run[i] = new Histogram(max_length + 1);
  }
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Sequence_characteristics.
 *
 *  arguments : stream, type de la variable (STATE / VALUE), histogramme
 *              des longueurs des sequences, flag niveau de detail, flag fichier.
 *
 *--------------------------------------------------------------*/

ostream& Sequence_characteristics::ascii_print(ostream &os , int type , const Histogram &hlength ,
                                               bool exhaustive , bool comment_flag) const

{
  register int i;


  if (exhaustive) {
    os << "\n";
    if (comment_flag) {
      os << "# ";
    }
    os << "  ";
    for (i = 0;i < nb_value;i++) {
      os << " | " << SEQ_label[SEQL_OBSERVED] << " "
         << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i;
    }
    os << " | " << STAT_label[STATL_FREQUENCY] << endl;

    index_value->ascii_print(os , comment_flag);
  }

  if (first_occurrence) {
    for (i = 0;i < nb_value;i++) {
      os << "\n";
      if (comment_flag) {
        os << "# ";
      }
      os << SEQ_label[type == STATE ? SEQL_STATE_FIRST_OCCURRENCE : SEQL_VALUE_FIRST_OCCURRENCE]
         << " " << i << " " << STAT_label[STATL_HISTOGRAM] << " - ";
      first_occurrence[i]->ascii_characteristic_print(os , false , comment_flag);

      if (exhaustive) {
        os << "\n";
        if (comment_flag) {
          os << "# ";
        }
        os << "   | " << SEQ_label[type == STATE ? SEQL_STATE_FIRST_OCCURRENCE : SEQL_VALUE_FIRST_OCCURRENCE]
           << " " << i << " " << STAT_label[STATL_HISTOGRAM] << endl;
        first_occurrence[i]->ascii_print(os , comment_flag);
      }
    }
  }

  for (i = 0;i < nb_value;i++) {
    os << "\n";
    if (comment_flag) {
      os << "# ";
    }
    os << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
       << SEQ_label[SEQL_RECURRENCE_TIME] << " " << STAT_label[STATL_HISTOGRAM] << " - ";
    recurrence_time[i]->ascii_characteristic_print(os , false , comment_flag);

    if ((recurrence_time[i]->nb_element > 0) && (exhaustive)) {
      os << "\n";
      if (comment_flag) {
        os << "# ";
      }
      os << "   | " << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
         << SEQ_label[SEQL_RECURRENCE_TIME] << " " << STAT_label[STATL_HISTOGRAM] << endl;
      recurrence_time[i]->ascii_print(os , comment_flag);
    }
  }

  for (i = 0;i < nb_value;i++) {
    os << "\n";
    if (comment_flag) {
      os << "# ";
    }
    os << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
       << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << " - ";
    sojourn_time[i]->ascii_characteristic_print(os , false , comment_flag);

    if ((sojourn_time[i]->nb_element > 0) && (exhaustive)) {
      os << "\n";
      if (comment_flag) {
        os << "# ";
      }
      os << "   | " << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
         << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << endl;
      sojourn_time[i]->ascii_print(os , comment_flag);
    }

    if (initial_run) {
      os << "\n";
      if (comment_flag) {
        os << "# ";
      }
      os << SEQ_label[SEQL_INITIAL_RUN] << " - "
         << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
         << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << " - ";
      initial_run[i]->ascii_characteristic_print(os , false , comment_flag);

      if ((initial_run[i]->nb_element > 0) && (exhaustive)) {
        os << "\n";
        if (comment_flag) {
          os << "# ";
        }
        os << "   | " << SEQ_label[SEQL_INITIAL_RUN] << " - "
           << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
           << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << endl;
        initial_run[i]->ascii_print(os , comment_flag);
      }
    }

    os << "\n";
    if (comment_flag) {
      os << "# ";
    }
    os << SEQ_label[SEQL_FINAL_RUN] << " - "
       << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
       << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << " - ";
    final_run[i]->ascii_characteristic_print(os , false , comment_flag);

    if ((final_run[i]->nb_element > 0) && (exhaustive)) {
      os << "\n";
      if (comment_flag) {
        os << "# ";
      }
      os << "   | " << SEQ_label[SEQL_FINAL_RUN] << " - "
         << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
         << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << endl;
      final_run[i]->ascii_print(os , comment_flag);
    }
  }

  if (nb_run) {
    for (i = 0;i < nb_value;i++) {
      os << "\n";
      if (comment_flag) {
        os << "# ";
      }
      os << SEQ_label[type == STATE ? SEQL_STATE_NB_RUN : SEQL_VALUE_NB_RUN]
         << " " << i << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM] << " - ";
      nb_run[i]->ascii_characteristic_print(os , (hlength.variance > 0. ? false : true) , comment_flag);

      if (exhaustive) {
        os << "\n";
        if (comment_flag) {
          os << "# ";
        }
        os << "   | " << SEQ_label[type == STATE ? SEQL_STATE_NB_RUN : SEQL_VALUE_NB_RUN]
           << " " << i << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM] << endl;
        nb_run[i]->ascii_print(os , comment_flag);
      }
    }
  }

  if (nb_occurrence) {
    for (i = 0;i < nb_value;i++) {
      os << "\n";
      if (comment_flag) {
        os << "# ";
      }
      os << SEQ_label[type == STATE ? SEQL_STATE_NB_OCCURRENCE : SEQL_VALUE_NB_OCCURRENCE]
         << " " << i << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM] << " - ";
      nb_occurrence[i]->ascii_characteristic_print(os , (hlength.variance > 0. ? false : true) , comment_flag);

      if (exhaustive) {
        os << "\n";
        if (comment_flag) {
          os << "# ";
        }
        os << "   | " << SEQ_label[type == STATE ? SEQL_STATE_NB_OCCURRENCE : SEQL_VALUE_NB_OCCURRENCE]
           << " " << i << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM] << endl;
        nb_occurrence[i]->ascii_print(os , comment_flag);
      }
    }
  }

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Sequence_characteristics au format tableur.
 *
 *  arguments : stream, type de la variable (STATE / VALUE), histogramme
 *              des longueurs des sequences.
 *
 *--------------------------------------------------------------*/

ostream& Sequence_characteristics::spreadsheet_print(ostream &os , int type ,
                                                     const Histogram &hlength) const

{
  register int i;
  Curves *smoothed_curves;


  os << "\n";
  for (i = 0;i < nb_value;i++) {
    os << "\t" << SEQ_label[SEQL_OBSERVED] << " "
       << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i;
  }
  os << "\t" << STAT_label[STATL_FREQUENCY] << endl;
  index_value->spreadsheet_print(os);

  smoothed_curves = new Curves(*index_value , 's');

  os << "\n" << SEQ_label[SEQL_SMOOTHED_OBSERVED_PROBABILITIES] << endl;
  for (i = 0;i < nb_value;i++) {
    os << "\t" << SEQ_label[SEQL_OBSERVED] << " "
       << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i;
  }
  os << "\t" << STAT_label[STATL_FREQUENCY] << endl;
  smoothed_curves->spreadsheet_print(os);

  delete smoothed_curves;

  if (first_occurrence) {
    for (i = 0;i < nb_value;i++) {
      os << "\n" << SEQ_label[type == STATE ? SEQL_STATE_FIRST_OCCURRENCE : SEQL_VALUE_FIRST_OCCURRENCE]
         << " " << i << " " << STAT_label[STATL_HISTOGRAM] << "\t";
      first_occurrence[i]->spreadsheet_characteristic_print(os);

      os << "\n\t" << SEQ_label[type == STATE ? SEQL_STATE_FIRST_OCCURRENCE : SEQL_VALUE_FIRST_OCCURRENCE]
         << " " << i << " " << STAT_label[STATL_HISTOGRAM] << endl;
      first_occurrence[i]->spreadsheet_print(os);
    }
  }

  for (i = 0;i < nb_value;i++) {
    os << "\n" << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
       << SEQ_label[SEQL_RECURRENCE_TIME] << " " << STAT_label[STATL_HISTOGRAM] << "\t";
    recurrence_time[i]->spreadsheet_characteristic_print(os);

    if (recurrence_time[i]->nb_element > 0) {
      os << "\n\t" << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
         << SEQ_label[SEQL_RECURRENCE_TIME] << " " << STAT_label[STATL_HISTOGRAM] << endl;
      recurrence_time[i]->spreadsheet_print(os);
    }
  }

  for (i = 0;i < nb_value;i++) {
    os << "\n" << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
       << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << "\t";
    sojourn_time[i]->spreadsheet_characteristic_print(os);

    if (sojourn_time[i]->nb_element > 0) {
      os << "\n\t" << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
         << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << endl;
      sojourn_time[i]->spreadsheet_print(os);
    }

    if (initial_run) {
      os << "\n" << SEQ_label[SEQL_INITIAL_RUN] << " - "
         << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
         << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << "\t";
      initial_run[i]->spreadsheet_characteristic_print(os);

      if (initial_run[i]->nb_element > 0) {
        os << "\n\t" << SEQ_label[SEQL_INITIAL_RUN] << " - "
           << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
           << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << endl;
        initial_run[i]->spreadsheet_print(os);
      }
    }

    os << "\n" << SEQ_label[SEQL_FINAL_RUN] << " - "
       << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
       << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << "\t";
    final_run[i]->spreadsheet_characteristic_print(os);

    if (final_run[i]->nb_element > 0) {
      os << "\n\t" << SEQ_label[SEQL_FINAL_RUN] << " - "
         << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << i << " "
         << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM] << endl;
      final_run[i]->spreadsheet_print(os);
    }
  }

  if (nb_run) {
    for (i = 0;i < nb_value;i++) {
      os << "\n" << SEQ_label[type == STATE ? SEQL_STATE_NB_RUN : SEQL_VALUE_NB_RUN]
         << " " << i << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM] << "\t";
      nb_run[i]->spreadsheet_characteristic_print(os , (hlength.variance > 0. ? false : true));

      os << "\n\t" << SEQ_label[type == STATE ? SEQL_STATE_NB_RUN : SEQL_VALUE_NB_RUN]
         << " " << i << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM] << endl;
      nb_run[i]->spreadsheet_print(os);
    }
  }

  if (nb_occurrence) {
    for (i = 0;i < nb_value;i++) {
      os << "\n" << SEQ_label[type == STATE ? SEQL_STATE_NB_OCCURRENCE : SEQL_VALUE_NB_OCCURRENCE]
         << " " << i << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM] << "\t";
      nb_occurrence[i]->spreadsheet_characteristic_print(os , (hlength.variance > 0. ? false : true));

      os << "\n\t" << SEQ_label[type == STATE ? SEQL_STATE_NB_OCCURRENCE : SEQL_VALUE_NB_OCCURRENCE]
         << " " << i << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM] << endl;
      nb_occurrence[i]->spreadsheet_print(os);
    }
  }

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Sortie Gnuplot d'un objet Sequences_characteristics.
 *
 *  arguments : prefixe des fichiers, titre des figures, indice de la variable,
 *              nombre de variables, type de la variable (STATE / VALUE),
 *              histogramme des longueurs des sequences.
 *
 *--------------------------------------------------------------*/

bool Sequence_characteristics::plot_print(const char *prefix , const char *title , int variable ,
                                          int nb_variable , int type , const Histogram &hlength) const

{
  bool status , start;
  register int i , j , k;
  int index_length , nb_histo , histo_index;
  Curves *pcurves;
  const Histogram **phisto;
  ostringstream data_file_name[2];


  // ecriture des fichiers de donnees

  data_file_name[0] << prefix << variable + 1 << 0 << ".dat";

  index_length = index_value->plot_length_computation();

  if (index_value->frequency[index_length - 1] < MAX_FREQUENCY) {
    pcurves = new Curves(*index_value , 's');
  }
  else {
    pcurves = 0;
  }

  status = index_value->plot_print((data_file_name[0].str()).c_str() , index_length , pcurves);
  delete pcurves;

  if (status) {
    phisto = new const Histogram*[1 + NB_OUTPUT * 6];

    data_file_name[1] << prefix << variable + 1 << 1 << ".dat";

    nb_histo = 0;
    if (first_occurrence) {
      for (i = 0;i < nb_value;i++) {
        phisto[nb_histo++] = first_occurrence[i];
      }
    }

    for (i = 0;i < nb_value;i++) {
      if (recurrence_time[i]->nb_element > 0) {
        phisto[nb_histo++] = recurrence_time[i];
      }
    }

    for (i = 0;i < nb_value;i++) {
      if (sojourn_time[i]->nb_element > 0) {
        phisto[nb_histo++] = sojourn_time[i];
      }
      if ((initial_run) && (initial_run[i]->nb_element > 0)) {
        phisto[nb_histo++] = initial_run[i];
      }
      if (final_run[i]->nb_element > 0) {
        phisto[nb_histo++] = final_run[i];
      }
    }

    if ((nb_run) && (nb_occurrence)) {
      for (i = 0;i < nb_value;i++) {
        phisto[nb_histo++] = nb_run[i];
        phisto[nb_histo++] = nb_occurrence[i];
      }
    }

    hlength.plot_print((data_file_name[1].str()).c_str() , nb_histo , phisto);

    // ecriture des fichiers de commandes et des fichiers d'impression

    for (i = 0;i < 2;i++) {
      ostringstream file_name[2];

      switch (i) {
      case 0 :
        file_name[0] << prefix << variable + 1 << 1 << ".plot";
        break;
      case 1 :
        file_name[0] << prefix << variable + 1 << 1 << ".print";
        break;
      }

      ofstream out_file((file_name[0].str()).c_str());

      if (i == 1) {
        out_file << "set terminal postscript" << endl;
        file_name[1] << label(prefix) << variable + 1 << 1 << ".ps";
        out_file << "set output \"" << file_name[1].str() << "\"\n\n";
      }

      out_file << "set border 15 lw 0\n" << "set tics out\n" << "set xtics nomirror\n";

      if (index_value->frequency[index_length - 1] < MAX_FREQUENCY) {
        out_file << "set title" << " \"";
        if (title) {
          out_file << title << " - ";
        }
        if (nb_variable > 1) {
          out_file << STAT_label[STATL_VARIABLE] << " " << variable + 1 << " - ";
        }
        out_file << SEQ_label[SEQL_SMOOTHED_OBSERVED_PROBABILITIES] << "\"\n\n";

        if (index_length - 1 < TIC_THRESHOLD) {
          out_file << "set xtics 0,1" << endl;
        }

        j = nb_value + 1;

        out_file << "plot [0:" << index_length - 1 << "] [0:1] ";
        for (k = 0;k < nb_value;k++) {
          out_file << "\"" << label((data_file_name[0].str()).c_str()) << "\" using "
                   << j++ << " title \"" << SEQ_label[SEQL_OBSERVED] << " "
                   << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " "
                   << k << "\" with linespoints";
          if (k < nb_value - 1) {
            out_file << ",\\";
          }
          out_file << endl;
        }

        if (index_length - 1 < TIC_THRESHOLD) {
          out_file << "set xtics autofreq" << endl;
        }

        if (i == 0) {
          out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
        }
        out_file << endl;
      }

      out_file << "set title \"";
      if (title) {
        out_file << title;
        if (nb_variable > 1) {
          out_file << " - ";
        }
      }
      if (nb_variable > 1) {
        out_file << STAT_label[STATL_VARIABLE] << " " << variable + 1;
      }
      out_file << "\"\n\n";

      if (index_length - 1 < TIC_THRESHOLD) {
        out_file << "set xtics 0,1" << endl;
      }

      out_file << "plot [0:" << index_length - 1 << "] [0:1] ";
      for (j = 0;j < nb_value;j++) {
        out_file << "\"" << label((data_file_name[0].str()).c_str()) << "\" using "
                 << j + 1 << " title \"" << SEQ_label[SEQL_OBSERVED] << " "
                 << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " "
                 << j << "\" with linespoints";
        if (j < nb_value - 1) {
          out_file << ",\\";
        }
        out_file << endl;
      }

      if (index_length - 1 < TIC_THRESHOLD) {
        out_file << "set xtics autofreq" << endl;
      }

      if (i == 0) {
        out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
      }
      out_file << endl;

      if (hlength.nb_value - 1 < TIC_THRESHOLD) {
        out_file << "set xtics 0,1" << endl;
      }
      if ((int)(hlength.max * YSCALE) + 1 < TIC_THRESHOLD) {
        out_file << "set ytics 0,1" << endl;
      }

      out_file << "plot [0:" << hlength.nb_value - 1 << "] [0:"
               << (int)(hlength.max * YSCALE) + 1 << "] \"" 
               << label((data_file_name[1].str()).c_str()) << "\" using 1 title \""
               << SEQ_label[SEQL_SEQUENCE_LENGTH] << " " << STAT_label[STATL_HISTOGRAM]
               << "\" with impulses" << endl;

      if (hlength.nb_value - 1 < TIC_THRESHOLD) {
        out_file << "set xtics autofreq" << endl;
      }
      if ((int)(hlength.max * YSCALE) + 1 < TIC_THRESHOLD) {
        out_file << "set ytics autofreq" << endl;
      }

      if (i == 1) {
        out_file << "\nset terminal x11" << endl;
      }

      out_file << "\npause 0 \"" << STAT_label[STATL_END] << "\"" << endl;
    }

    histo_index = 2;

    if (first_occurrence) {
      for (i = 0;i < 2;i++) {
        ostringstream file_name[2];

        switch (i) {
        case 0 :
          file_name[0] << prefix << variable + 1 << 2 << ".plot";
          break;
        case 1 :
          file_name[0] << prefix << variable + 1 << 2 << ".print";
          break;
        }

        ofstream out_file((file_name[0].str()).c_str());

        if (i == 1) {
          out_file << "set terminal postscript" << endl;
          file_name[1] << label(prefix) << variable + 1 << 2 << ".ps";
          out_file << "set output \"" << file_name[1].str() << "\"\n\n";
        }

        out_file << "set border 15 lw 0\n" << "set tics out\n" << "set xtics nomirror\n"
                 << "set title";
        if ((title) || (nb_variable > 1)) {
          out_file << " \"";
          if (title) {
            out_file << title;
            if (nb_variable > 1) {
              out_file << " - ";
            }
          }
          if (nb_variable > 1) {
            out_file << STAT_label[STATL_VARIABLE] << " " << variable + 1;
          }
          out_file << "\"";
        }
        out_file << "\n\n";

        j = histo_index;

        start = true;
        for (k = 0;k < nb_value;k++) {
          if (!start) {
            if (i == 0) {
              out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
            }
            out_file << endl;
          }
          else {
            start = false;
          }

          if (MAX(1 , first_occurrence[k]->nb_value - 1) < TIC_THRESHOLD) {
            out_file << "set xtics 0,1" << endl;
          }
          if ((int)(first_occurrence[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics 0,1" << endl;
          }

          out_file << "plot [0:" << MAX(first_occurrence[k]->nb_value - 1 , 1) << "] [0:"
                   << (int)(first_occurrence[k]->max * YSCALE) + 1 << "] \""
                   << label((data_file_name[1].str()).c_str()) << "\" using " << j++
                   << " title \"" << SEQ_label[type == STATE ? SEQL_STATE_FIRST_OCCURRENCE : SEQL_VALUE_FIRST_OCCURRENCE]
                   << " " << k << " " << STAT_label[STATL_HISTOGRAM] << "\" with impulses" << endl;

          if (MAX(1 , first_occurrence[k]->nb_value - 1) < TIC_THRESHOLD) {
            out_file << "set xtics autofreq" << endl;
          }
          if ((int)(first_occurrence[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics autofreq" << endl;
          }
        }

        if (i == 1) {
          out_file << "\nset terminal x11" << endl;
        }

        out_file << "\npause 0 \"" << STAT_label[STATL_END] << "\"" << endl;
      }

      histo_index = j;
    }

    for (i = 0;i < 2;i++) {
      ostringstream file_name[2];

      switch (i) {
      case 0 :
        file_name[0] << prefix << variable + 1 << 3 << ".plot";
        break;
      case 1 :
        file_name[0] << prefix << variable + 1 << 3 << ".print";
        break;
      }

      ofstream out_file((file_name[0].str()).c_str());

      if (i == 1) {
        out_file << "set terminal postscript" << endl;
        file_name[1] << label(prefix) << variable + 1 << 3 << ".ps";
        out_file << "set output \"" << file_name[1].str() << "\"\n\n";
      }

      out_file << "set border 15 lw 0\n" << "set tics out\n" << "set xtics nomirror\n"
               << "set title";
      if ((title) || (nb_variable > 1)) {
        out_file << " \"";
        if (title) {
          out_file << title;
          if (nb_variable > 1) {
            out_file << " - ";
          }
        }
        if (nb_variable > 1) {
          out_file << STAT_label[STATL_VARIABLE] << " " << variable + 1;
        }
        out_file << "\"";
      }
      out_file << "\n\n";

      j = histo_index;

      start = true;
      for (k = 0;k < nb_value;k++) {
        if (recurrence_time[k]->nb_element > 0) {
          if (!start) {
            if (i == 0) {
              out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
            }
            out_file << endl;
          }
          else {
            start = false;
          }

          if (recurrence_time[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics 0,1" << endl;
          }
          if ((int)(recurrence_time[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics 0,1" << endl;
          }

          out_file << "plot [0:" << recurrence_time[k]->nb_value - 1 << "] [0:"
                   << (int)(recurrence_time[k]->max * YSCALE) + 1 << "] \""
                   << label((data_file_name[1].str()).c_str()) << "\" using " << j++
                   << " title \"" << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE]
                   << " " << k << " " << SEQ_label[SEQL_RECURRENCE_TIME] << " " << STAT_label[STATL_HISTOGRAM]
                   << "\" with impulses" << endl;

          if (recurrence_time[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics autofreq" << endl;
          }
          if ((int)(recurrence_time[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics autofreq" << endl;
          }
        }
      }

      if (i == 1) {
        out_file << "\nset terminal x11" << endl;
      }

      out_file << "\npause 0 \"" << STAT_label[STATL_END] << "\"" << endl;
    }

    histo_index = j;

    for (i = 0;i < 2;i++) {
      ostringstream file_name[2];

      switch (i) {
      case 0 :
        file_name[0] << prefix << variable + 1 << 4 << ".plot";
        break;
      case 1 :
        file_name[0] << prefix << variable + 1 << 4 << ".print";
        break;
      }

      ofstream out_file((file_name[0].str()).c_str());

      if (i == 1) {
        out_file << "set terminal postscript" << endl;
        file_name[1] << label(prefix) << variable + 1 << 4 << ".ps";
        out_file << "set output \"" << file_name[1].str() << "\"\n\n";
      }

      out_file << "set border 15 lw 0\n" << "set tics out\n" << "set xtics nomirror\n"
               << "set title";
      if ((title) || (nb_variable > 1)) {
        out_file << " \"";
        if (title) {
          out_file << title;
          if (nb_variable > 1) {
            out_file << " - ";
          }
        }
        if (nb_variable > 1) {
          out_file << STAT_label[STATL_VARIABLE] << " " << variable + 1;
        }
        out_file << "\"";
      }
      out_file << "\n\n";

      j = histo_index;

      start = true;
      for (k = 0;k < nb_value;k++) {
        if (sojourn_time[k]->nb_element > 0) {
          if (!start) {
            if (i == 0) {
              out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
            }
            out_file << endl;
          }
          else {
            start = false;
          }

          if (sojourn_time[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics 0,1" << endl;
          }
          if ((int)(sojourn_time[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics 0,1" << endl;
          }

          out_file << "plot [0:" << sojourn_time[k]->nb_value - 1 << "] [0:"
                   << (int)(sojourn_time[k]->max * YSCALE) + 1 << "] \""
                   << label((data_file_name[1].str()).c_str()) << "\" using " << j++
                   << " title \"" << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << k
                   << " " << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM]
                   << "\" with impulses" << endl;

          if (sojourn_time[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics autofreq" << endl;
          }
          if ((int)(sojourn_time[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics autofreq" << endl;
          }
        }

        if ((initial_run) && (initial_run[k]->nb_element > 0)) {
          if (!start) {
            if (i == 0) {
              out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
            }
            out_file << endl;
          }
          else {
            start = false;
          }

          if (initial_run[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics 0,1" << endl;
          }
          if ((int)(initial_run[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics 0,1" << endl;
          }

          out_file << "plot [0:" << initial_run[k]->nb_value - 1 << "] [0:"
                   << (int)(initial_run[k]->max * YSCALE) + 1 << "] \""
                   << label((data_file_name[1].str()).c_str()) << "\" using " << j++
                   << " title \"" << SEQ_label[SEQL_INITIAL_RUN] << " - "
                   << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << k
                   << " " << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM]
                   << "\" with impulses" << endl;

          if (initial_run[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics autofreq" << endl;
          }
          if ((int)(initial_run[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics autofreq" << endl;
          }
        }

        if (final_run[k]->nb_element > 0) {
          if (!start) {
            if (i == 0) {
              out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
            }
            out_file << endl;
          }
          else {
            start = false;
          }

          if (final_run[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics 0,1" << endl;
          }
          if ((int)(final_run[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics 0,1" << endl;
          }

          out_file << "plot [0:" << final_run[k]->nb_value - 1 << "] [0:"
                   << (int)(final_run[k]->max * YSCALE) + 1 << "] \""
                   << label((data_file_name[1].str()).c_str()) << "\" using " << j++
                   << " title \"" << SEQ_label[SEQL_FINAL_RUN] << " - "
                   << STAT_label[type == STATE ? STATL_STATE : STATL_VALUE] << " " << k
                   << " " << SEQ_label[SEQL_SOJOURN_TIME] << " " << STAT_label[STATL_HISTOGRAM]
                   << "\" with impulses" << endl;

          if (final_run[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics autofreq" << endl;
          }
          if ((int)(final_run[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics autofreq" << endl;
          }
        }
      }

      if (i == 1) {
        out_file << "\nset terminal x11" << endl;
      }

      out_file << "\npause 0 \"" << STAT_label[STATL_END] << "\"" << endl;
    }

    histo_index = j;

    if ((nb_run) && (nb_occurrence)) {
      for (i = 0;i < 2;i++) {
        ostringstream file_name[2];

        switch (i) {
        case 0 :
          file_name[0] << prefix << variable + 1 << 5 << ".plot";
          break;
        case 1 :
          file_name[0] << prefix << variable + 1 << 5 << ".print";
          break;
        }

        ofstream out_file((file_name[0].str()).c_str());

        if (i == 1) {
          out_file << "set terminal postscript" << endl;
          file_name[1] << label(prefix) << variable + 1 << 5 << ".ps";
          out_file << "set output \"" << file_name[1].str() << "\"\n\n";
        }

        out_file << "set border 15 lw 0\n" << "set tics out\n" << "set xtics nomirror\n"
                 << "set title";
        if ((title) || (nb_variable > 1)) {
          out_file << " \"";
          if (title) {
            out_file << title;
            if (nb_variable > 1) {
              out_file << " - ";
            }
          }
          if (nb_variable > 1) {
            out_file << STAT_label[STATL_VARIABLE] << " " << variable + 1;
          }
          out_file << "\"";
        }
        out_file << "\n\n";

        j = histo_index;

        for (k = 0;k < nb_value;k++) {
          if (nb_run[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics 0,1" << endl;
          }
          if ((int)(nb_run[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics 0,1" << endl;
          }

          out_file << "plot [0:" << nb_run[k]->nb_value - 1 << "] [0:"
                   << (int)(nb_run[k]->max * YSCALE) + 1 << "] \""
                   << label((data_file_name[1].str()).c_str()) << "\" using " << j++
                   << " title \"" << SEQ_label[type == STATE ? SEQL_STATE_NB_RUN : SEQL_VALUE_NB_RUN]
                   << " " << k << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM]
                   << "\" with impulses" << endl;

          if (nb_run[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics autofreq" << endl;
          }
          if ((int)(nb_run[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics autofreq" << endl;
          }

          if (i == 0) {
            out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
          }
          out_file << endl;

          if (nb_occurrence[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics 0,1" << endl;
          }
          if ((int)(nb_occurrence[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics 0,1" << endl;
          }

          out_file << "plot [0:" << nb_occurrence[k]->nb_value - 1 << "] [0:"
                   << (int)(nb_occurrence[k]->max * YSCALE) + 1 << "] \""
                   << label((data_file_name[1].str()).c_str()) << "\" using " << j++
                   << " title \"" << SEQ_label[type == STATE ? SEQL_STATE_NB_OCCURRENCE : SEQL_VALUE_NB_OCCURRENCE]
                   << " " << k << " " << SEQ_label[SEQL_PER_SEQUENCE] << " " << STAT_label[STATL_HISTOGRAM]
                   << "\" with impulses" << endl;

          if (nb_occurrence[k]->nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics autofreq" << endl;
          }
          if ((int)(nb_occurrence[k]->max * YSCALE) + 1 < TIC_THRESHOLD) {
            out_file << "set ytics autofreq" << endl;
          }

          if (i == 0) {
            out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
          }
          out_file << endl;
        }

        if (hlength.nb_value - 1 < TIC_THRESHOLD) {
          out_file << "set xtics 0,1" << endl;
        }
        if ((int)(hlength.max * YSCALE) + 1 < TIC_THRESHOLD) {
          out_file << "set ytics 0,1" << endl;
        }

        out_file << "plot [0:" << hlength.nb_value - 1 << "] [0:"
                 << (int)(hlength.max * YSCALE) + 1 << "] \"" 
                 << label((data_file_name[1].str()).c_str()) << "\" using 1 title \""
                 << SEQ_label[SEQL_SEQUENCE_LENGTH] << " " << STAT_label[STATL_HISTOGRAM]
                 << "\" with impulses" << endl;

        if (hlength.nb_value - 1 < TIC_THRESHOLD) {
          out_file << "set xtics autofreq" << endl;
        }
        if ((int)(hlength.max * YSCALE) + 1 < TIC_THRESHOLD) {
          out_file << "set ytics autofreq" << endl;
        }

        if (i == 1) {
          out_file << "\nset terminal x11" << endl;
        }

        out_file << "\npause 0 \"" << STAT_label[STATL_END] << "\"" << endl;
      }
    }

    delete [] phisto;
  }

  return status;
}


/*--------------------------------------------------------------*
 *
 *  Fonctions pour la persistance.
 *
 *--------------------------------------------------------------*/

/* RWspace Sequence_characteristics::binaryStoreSize() const

{
  register int i;
  RWspace size;


  size = sizeof(nb_value)+ index_value->binaryStoreSize();

  for (i = 0;i < nb_value;i++) {
    size += first_occurrence[i]->binaryStoreSize();
  }

  for (i = 0;i < nb_value;i++) {
    size += recurrence_time[i]->binaryStoreSize();
  }

  for (i = 0;i < nb_value;i++) {
    size += sojourn_time[i]->binaryStoreSize();
  }

  size += sizeof(true);
  if (initial_run) {
    for (i = 0;i < nb_value;i++) {
      size += initial_run[i]->binaryStoreSize();
    }
  }

  for (i = 0;i < nb_value;i++) {
    size += final_run[i]->binaryStoreSize();
  }

  for (i = 0;i < nb_value;i++) {
    size += nb_run[i]->binaryStoreSize();
  }

  for (i = 0;i < nb_value;i++) {
    size += nb_occurrence[i]->binaryStoreSize();
  }

  return size;
}


void Sequence_characteristics::restoreGuts(RWvistream &is)

{
  bool status;
  register int i;


  remove();

  is >> nb_value;

  index_value = new Curves();
  index_value->restoreGuts(is);

  first_occurrence = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    first_occurrence[i] = new Histogram();
    first_occurrence[i]->restoreGuts(is);
  }

  recurrence_time = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    recurrence_time[i] = new Histogram();
    recurrence_time[i]->restoreGuts(is);
  }

  sojourn_time = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    sojourn_time[i] = new Histogram();
    sojourn_time[i]->restoreGuts(is);
  }

  is >> status;
  if (status) {
    initial_run = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      initial_run[i] = new Histogram();
      initial_run[i]->restoreGuts(is);
    }
  }
  else {
    initial_run = 0;
  }

  final_run = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    final_run[i] = new Histogram();
    final_run[i]->restoreGuts(is);
  }

  nb_run = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    nb_run[i] = new Histogram();
    nb_run[i]->restoreGuts(is);
  }

  nb_occurrence = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    nb_occurrence[i] = new Histogram();
    nb_occurrence[i]->restoreGuts(is);
  }
}


void Sequence_characteristics::restoreGuts(RWFile &file)

{
  bool status;
  register int i;


  remove();

  file.Read(nb_value);

  index_value = new Curves();
  index_value->restoreGuts(file);

  first_occurrence = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    first_occurrence[i] = new Histogram();
    first_occurrence[i]->restoreGuts(file);
  }

  recurrence_time = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    recurrence_time[i] = new Histogram();
    recurrence_time[i]->restoreGuts(file);
  }

  sojourn_time = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    sojourn_time[i] = new Histogram();
    sojourn_time[i]->restoreGuts(file);
  }

  file.Read(status);
  if (status) {
    initial_run = new Histogram*[nb_value];
    for (i = 0;i < nb_value;i++) {
      initial_run[i] = new Histogram();
      initial_run[i]->restoreGuts(file);
    }
  }
  else {
    initial_run = 0;
  }

  final_run = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    final_run[i] = new Histogram();
    final_run[i]->restoreGuts(file);
  }

  nb_run = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    nb_run[i] = new Histogram();
    nb_run[i]->restoreGuts(file);
  }

  nb_occurrence = new Histogram*[nb_value];
  for (i = 0;i < nb_value;i++) {
    nb_occurrence[i] = new Histogram();
    nb_occurrence[i]->restoreGuts(file);
  }
}


void Sequence_characteristics::saveGuts(RWvostream &os) const

{
  register int i;


  os << nb_value;

  index_value->saveGuts(os);

  for (i = 0;i < nb_value;i++) {
    first_occurrence[i]->saveGuts(os);
  }

  for (i = 0;i < nb_value;i++) {
    recurrence_time[i]->saveGuts(os);
  }

  for (i = 0;i < nb_value;i++) {
    sojourn_time[i]->saveGuts(os);
  }

  if (initial_run) {
    os << true;
    for (i = 0;i < nb_value;i++) {
      initial_run[i]->saveGuts(os);
    }
  }
  else {
    os << false;
  }

  for (i = 0;i < nb_value;i++) {
    final_run[i]->saveGuts(os);
  }

  for (i = 0;i < nb_value;i++) {
    nb_run[i]->saveGuts(os);
  }

  for (i = 0;i < nb_value;i++) {
    nb_occurrence[i]->saveGuts(os);
  }
}


void Sequence_characteristics::saveGuts(RWFile &file) const

{
  register int i;


  file.Write(nb_value);

  index_value->saveGuts(file);

  for (i = 0;i < nb_value;i++) {
    first_occurrence[i]->saveGuts(file);
  }

  for (i = 0;i < nb_value;i++) {
    recurrence_time[i]->saveGuts(file);
  }

  for (i = 0;i < nb_value;i++) {
    sojourn_time[i]->saveGuts(file);
  }

  if (initial_run) {
    file.Write(true);
    for (i = 0;i < nb_value;i++) {
      initial_run[i]->saveGuts(file);
    }
  }
  else {
    file.Write(false);
  }

  for (i = 0;i < nb_value;i++) {
    final_run[i]->saveGuts(file);
  }

  for (i = 0;i < nb_value;i++) {
    nb_run[i]->saveGuts(file);
  }

  for (i = 0;i < nb_value;i++) {
    nb_occurrence[i]->saveGuts(file);
  }
} */
