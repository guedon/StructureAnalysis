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



#include <math.h>
#include <sstream>
#include "tool/rw_tokenizer.h"
#include "tool/rw_cstring.h"
#include "tool/rw_locale.h"
// #include <rw/vstream.h>
// #include <rw/rwfile.h>
#include "stat_tool/stat_tools.h"
#include "stat_tool/curves.h"
#include "stat_tool/markovian.h"
#include "stat_tool/stat_label.h"
#include "sequences.h"
#include "semi_markov.h"
#include "hidden_semi_markov.h"
#include "sequence_label.h"

using namespace std;



/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe Semi_markov.
 *
 *  arguments : pointeur sur un objet Chain et sur un objet Nonparametric_sequence_process,
 *              nombre de processus d'observation, pointeurs sur
 *              des objets Nonparametric_process, longueur des sequences,
 *              flag sur le calcul des lois de comptage.
 *
 *--------------------------------------------------------------*/

Semi_markov::Semi_markov(const Chain *pchain , const Nonparametric_sequence_process *poccupancy ,
                         int inb_output_process , Nonparametric_process **pobservation ,
                         int length , bool counting_flag)
:Chain(*pchain)

{
  register int i;


  nb_iterator = 0;
  semi_markov_data = 0;

  nb_output_process = inb_output_process;

  nonparametric_process = new Nonparametric_sequence_process*[nb_output_process + 1];
  nonparametric_process[0] = new Nonparametric_sequence_process(*poccupancy);
  for (i = 1;i <= nb_output_process;i++) {
    nonparametric_process[i] = new Nonparametric_sequence_process(*pobservation[i - 1]);
  }

  parametric_process = 0;

  for (i = 0;i < nb_state;i++) {
    if (transition[i][i] < 1.) {
      nonparametric_process[0]->absorption[i] = 0.;
    }
    else {
      nonparametric_process[0]->absorption[i] = 1.;
    }
  }

  state_subtype = new int[nb_state];
  forward = new Forward*[nb_state];

  for (i = 0;i < nb_state;i++) {
    state_subtype[i] = (nonparametric_process[0]->sojourn_time[i] ? SEMI_MARKOVIAN : MARKOVIAN);

    if ((state_subtype[i] == SEMI_MARKOVIAN) && (state_type[i] == 'r')) {
      forward[i] = new Forward(*(nonparametric_process[0]->sojourn_time[i]));
    }
    else {
      forward[i] = 0;
    }
  }

  if (type == 'e') {
    for (i = 0;i < nb_state;i++) {
      initial[i] = 1. / (double)nb_state;
    }
    initial_probability_computation();
  }

  if (length > COUNTING_MAX_LENGTH) {
    counting_flag = false;
  }
  characteristic_computation(length , counting_flag);
}


/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe Semi_markov.
 *
 *  arguments : pointeur sur un objet Chain et sur un objet Nonparametric_sequence_process,
 *              nombre de processus d'observation, pointeurs sur des objets
 *              Nonparametric_process et Parametric_process, longueur des sequences,
 *              flag sur le calcul des lois de comptage.
 *
 *--------------------------------------------------------------*/

Semi_markov::Semi_markov(const Chain *pchain , const Nonparametric_sequence_process *poccupancy ,
                         int inb_output_process , Nonparametric_process **nonparametric_observation ,
                         Parametric_process **parametric_observation , int length , bool counting_flag)
:Chain(*pchain)

{
  register int i;


  nb_iterator = 0;
  semi_markov_data = 0;

  nb_output_process = inb_output_process;

  nonparametric_process = new Nonparametric_sequence_process*[nb_output_process + 1];
  nonparametric_process[0] = new Nonparametric_sequence_process(*poccupancy);
  parametric_process = new Parametric_process*[nb_output_process + 1];
  parametric_process[0] = 0;

  for (i = 1;i <= nb_output_process;i++) {
    if (nonparametric_observation[i - 1]) {
      nonparametric_process[i] = new Nonparametric_sequence_process(*nonparametric_observation[i - 1]);
      parametric_process[i] = 0;
    }
    else {
      nonparametric_process[i] = 0;
      parametric_process[i] = new Parametric_process(*parametric_observation[i - 1]);
    }
  }

  for (i = 0;i < nb_state;i++) {
    if (transition[i][i] < 1.) {
      nonparametric_process[0]->absorption[i] = 0.;
    }
    else {
      nonparametric_process[0]->absorption[i] = 1.;
    }
  }

  state_subtype = new int[nb_state];
  forward = new Forward*[nb_state];

  for (i = 0;i < nb_state;i++) {
    state_subtype[i] = (nonparametric_process[0]->sojourn_time[i] ? SEMI_MARKOVIAN : MARKOVIAN);

    if ((state_subtype[i] == SEMI_MARKOVIAN) && (state_type[i] == 'r')) {
      forward[i] = new Forward(*(nonparametric_process[0]->sojourn_time[i]));
    }
    else {
      forward[i] = 0;
    }
  }

  if (type == 'e') {
    for (i = 0;i < nb_state;i++) {
      initial[i] = 1. / (double)nb_state;
    }
    initial_probability_computation();
  }

  if (length > COUNTING_MAX_LENGTH) {
    counting_flag = false;
  }
  characteristic_computation(length , counting_flag);
}


/*--------------------------------------------------------------*
 *
 *  Destructeur de la classe Hidden_semi_markov.
 *
 *--------------------------------------------------------------*/

Hidden_semi_markov::~Hidden_semi_markov() {}


/*--------------------------------------------------------------*
 *
 *  Application d'un seuil sur les parametres d'une semi-chaine de Markov cachee.
 *
 *  argument : probabilite minimum.
 *
 *--------------------------------------------------------------*/

Hidden_semi_markov* Hidden_semi_markov::thresholding(double min_probability) const

{
  register int i;
  Hidden_semi_markov *hsmarkov;


  hsmarkov = new Hidden_semi_markov(*this , false , false);

  hsmarkov->Chain::thresholding(min_probability);
  hsmarkov->component_computation();

  for (i = 1;i <= nb_output_process;i++) {
    if (nonparametric_process[i]) {
      nonparametric_process[i]->thresholding(min_probability);
    }
  }

  return hsmarkov;
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet Hidden_semi_markov a partir d'un fichier.
 *
 *  arguments : reference sur un objet Format_error, path, longueur des sequences,
 *              flag sur le calcul des lois de comptage, seuil sur les fonctions de repartition
 *              des lois parametriques, flag format des processus d'observation.
 *
 *--------------------------------------------------------------*/

Hidden_semi_markov* hidden_semi_markov_ascii_read(Format_error &error , const char *path ,
                                                  int length , bool counting_flag ,
                                                  double cumul_threshold , bool old_format)

{
  RWLocaleSnapshot locale("en");
  RWCString buffer , token;
  size_t position;
  char type = 'v';
  bool status , lstatus , nonparametric;
  register int i;
  int line , order = 1 , nb_output_process , index;
  long value;
  const Chain *chain;
  const Nonparametric_sequence_process *occupancy;
  Nonparametric_process **nonparametric_observation;
  Parametric_process **parametric_observation;
  Hidden_semi_markov *hsmarkov;
  ifstream in_file(path);


  hsmarkov = 0;
  error.init();

  if (!in_file) {
    error.update(STAT_error[STATR_FILE_NAME]);
  }

  else {
    status = true;
    line = 0;

    if (length < 2) {
      status = false;
      error.update(SEQ_error[SEQR_SHORT_SEQUENCE_LENGTH]);
    }
    if (length > MAX_LENGTH) {
      status = false;
      error.update(SEQ_error[SEQR_LONG_SEQUENCE_LENGTH]);
    }

    while (buffer.readLine(in_file , false)) {
      line++;

#     ifdef DEBUG
      cout << line << "  " << buffer << endl;
#     endif

      position = buffer.first('#');
      if (position != RW_NPOS) {
        buffer.remove(position);
      }
      i = 0;

      RWCTokenizer next(buffer);

      while (!((token = next()).isNull())) {

        // test mot cle (EQUILIBRIUM) HIDDEN_SEMI-MARKOV_CHAIN

        if (i == 0) {
          if (token == SEQ_word[SEQW_HIDDEN_SEMI_MARKOV_CHAIN]) {
            type = 'o';
          }
          else if (token == SEQ_word[SEQW_EQUILIBRIUM_HIDDEN_SEMI_MARKOV_CHAIN]) {
            type = 'e';
          }
          else {
            status = false;
            ostringstream correction_message;
            correction_message << SEQ_word[SEQW_HIDDEN_SEMI_MARKOV_CHAIN] << " or "
                               << SEQ_word[SEQW_EQUILIBRIUM_HIDDEN_SEMI_MARKOV_CHAIN];
            error.correction_update(STAT_parsing[STATP_KEY_WORD] , (correction_message.str()).c_str() , line);
          }
        }

        i++;
      }

      if (i > 0) {
        if (i != 1) {
          status = false;
          error.update(STAT_parsing[STATP_FORMAT] , line);
        }
        break;
      }
    }

    if (type != 'v') {

      // analyse du format et lecture de la chaine de Markov

      chain = chain_parsing(error , in_file , line , type , false , order);

      if (chain) {

        // analyse du format et lecture des lois d'occupation de etats

        occupancy = occupancy_parsing(error , in_file , line , *chain , cumul_threshold);
        if (!occupancy) {
          status = false;
        }

        // analyse du format et lecture des lois d'observation

        switch (old_format) {

        case false : {
          nb_output_process = I_DEFAULT;

          nonparametric_observation = 0;
          parametric_observation = 0;

          while (buffer.readLine(in_file , false)) {
            line++;

#           ifdef DEBUG
            cout << line << "  " << buffer << endl;
#           endif

            position = buffer.first('#');
            if (position != RW_NPOS) {
              buffer.remove(position);
            }
            i = 0;

            RWCTokenizer next(buffer);

            while (!((token = next()).isNull())) {
              switch (i) {

              // test nombre de processus d'observation

              case 0 : {
                lstatus = locale.stringToNum(token , &value);
                if (lstatus) {
                  if ((value < 1) || (value > NB_OUTPUT_PROCESS)) {
                    lstatus = false;
                  }
                  else {
                    nb_output_process = value;
                  }
                }

                if (!lstatus) {
                  status = false;
                  error.update(STAT_parsing[STATP_NB_OUTPUT_PROCESS] , line , i + 1);
                }
                break;
              }

              // test mot cle OUTPUT_PROCESS(ES)

              case 1 : {
                if (token != STAT_word[nb_output_process == 1 ? STATW_OUTPUT_PROCESS : STATW_OUTPUT_PROCESSES]) {
                  status = false;
                  error.correction_update(STAT_parsing[STATP_KEY_WORD] ,
                                          STAT_word[nb_output_process == 1 ? STATW_OUTPUT_PROCESS : STATW_OUTPUT_PROCESSES] , line , i + 1);
                }
                break;
              }
              }

              i++;
            }

            if (i > 0) {
              if (i != 2) {
                status = false;
                error.update(STAT_parsing[STATP_FORMAT] , line);
              }
              break;
            }
          }

          if (nb_output_process == I_DEFAULT) {
            status = false;
            error.update(STAT_parsing[STATP_FORMAT] , line);
          }

          else {
            nonparametric_observation = new Nonparametric_process*[nb_output_process];
            parametric_observation = new Parametric_process*[nb_output_process];
            for (i = 0;i < nb_output_process;i++) {
              nonparametric_observation[i] = 0;
              parametric_observation[i] = 0;
            }

            index = 0;

            while (buffer.readLine(in_file , false)) {
              line++;

#             ifdef DEBUG
              cout << line << "  " << buffer << endl;
#             endif

              position = buffer.first('#');
              if (position != RW_NPOS) {
                buffer.remove(position);
              }
              i = 0;

              RWCTokenizer next(buffer);

              while (!((token = next()).isNull())) {
                switch (i) {

                // test mot cle OUTPUT_PROCESS

                case 0 : {
                  nonparametric = true;

                  if (token == STAT_word[STATW_OUTPUT_PROCESS]) {
                    index++;
                  }
                  else {
                    status = false;
                    error.correction_update(STAT_parsing[STATP_KEY_WORD] , STAT_word[STATW_OUTPUT_PROCESS] , line , i + 1);
                  }
                  break;
                }

                // test indice du processus d'observation

                case 1 : {
                  lstatus = locale.stringToNum(token , &value);
                  if ((lstatus) && ((value != index) || (value > nb_output_process))) {
                    lstatus = false;
                  }

                  if (!lstatus) {
                    status = false;
                    error.update(STAT_parsing[STATP_OUTPUT_PROCESS_INDEX] , line , i + 1);
                  }
                  break;
                }

                // test separateur

                case 2 : {
                  if (token != ":") {
                    status = false;
                    error.update(STAT_parsing[STATP_SEPARATOR] , line , i + 1);
                  }
                  break;
                }

                // test mot cle NONPARAMETRIC / PARAMETRIC

                case 3 : {
                  if (token == STAT_word[STATW_NONPARAMETRIC]) {
                    nonparametric = true;
                  }
                  else if (token == STAT_word[STATW_PARAMETRIC]) {
                    nonparametric = false;
                  }
                  else {
                    status = false;
                    ostringstream correction_message;
                    correction_message << STAT_word[STATW_NONPARAMETRIC] << " or "
                                       << STAT_word[STATW_PARAMETRIC];
                    error.correction_update(STAT_parsing[STATP_KEY_WORD] , (correction_message.str()).c_str() , line , i + 1);
                  }
                  break;
                }
                }

                i++;
              }

              if (i > 0) {
                if (i != 4) {
                  status = false;
                  error.update(STAT_parsing[STATP_FORMAT] , line);
                }

                switch (nonparametric) {

                case true : {
                  nonparametric_observation[index - 1] = observation_parsing(error , in_file , line ,
                                                                             chain->nb_state , true);
                  if (!nonparametric_observation[index - 1]) {
                    status = false;
                  }
                  break;
                }

                case false : {
                  parametric_observation[index - 1] = observation_parsing(error , in_file , line ,
                                                                          chain->nb_state ,
                                                                          cumul_threshold);
                  if (!parametric_observation[index - 1]) {
                    status = false;
                  }
                  break;
                }
                }
              }
            }

            if (index != nb_output_process) {
              status = false;
              error.update(STAT_parsing[STATP_FORMAT] , line);
            }

            if (status) {
              hsmarkov = new Hidden_semi_markov(chain , occupancy , nb_output_process ,
                                                nonparametric_observation , parametric_observation ,
                                                length , counting_flag);
            }

            for (i = 0;i < nb_output_process;i++) {
              delete nonparametric_observation[i];
              delete parametric_observation[i];
            }
            delete [] nonparametric_observation;
            delete [] parametric_observation;
          }
          break;
        }

        case true : {
          nonparametric_observation = old_observation_parsing(error , in_file , line ,
                                                              chain->nb_state ,
                                                              nb_output_process);

          if (nonparametric_observation) {
            if (status) {
              hsmarkov = new Hidden_semi_markov(chain , occupancy , nb_output_process ,
                                                nonparametric_observation , length , counting_flag);
            }

            for (i = 0;i < nb_output_process;i++) {
              delete nonparametric_observation[i];
            }
            delete [] nonparametric_observation;
          }
          break;
        }
        }

        delete chain;
        delete occupancy;
      }
    }
  }

  return hsmarkov;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Hidden_semi_markov dans un fichier.
 *
 *  arguments : stream, flag niveau de detail.
 *
 *--------------------------------------------------------------*/

ostream& Hidden_semi_markov::ascii_write(ostream &os , bool exhaustive) const

{
  Semi_markov::ascii_write(os , semi_markov_data , exhaustive ,
                           false , true);

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Hidden_semi_markov dans un fichier.
 *
 *  arguments : reference sur un objet Format_error, path,
 *              flag niveau de detail.
 *
 *--------------------------------------------------------------*/

bool Hidden_semi_markov::ascii_write(Format_error &error , const char *path ,
                                     bool exhaustive) const

{
  bool status;
  ofstream out_file(path);


  error.init();

  if (!out_file) {
    status = false;
    error.update(STAT_error[STATR_FILE_NAME]);
  }

  else {
    status = true;
    Semi_markov::ascii_write(out_file , semi_markov_data , exhaustive ,
                             true , true);
  }

  return status;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Hidden_semi_markov dans un fichier au format tableur.
 *
 *  arguments : reference sur un objet Format_error, path.
 *
 *--------------------------------------------------------------*/

bool Hidden_semi_markov::spreadsheet_write(Format_error &error , const char *path) const

{
  bool status;
  ofstream out_file(path);


  error.init();

  if (!out_file) {
    status = false;
    error.update(STAT_error[STATR_FILE_NAME]);
  }

  else {
    status = true;
    Semi_markov::spreadsheet_write(out_file , semi_markov_data , true);
  }

  return status;
}


/*--------------------------------------------------------------*
 *
 *  Fonctions pour la persistance.
 *
 *--------------------------------------------------------------*/

/* RWDEFINE_COLLECTABLE(Hidden_semi_markov , STATI_HIDDEN_SEMI_MARKOV);


RWspace Hidden_semi_markov::binaryStoreSize() const

{
  return Semi_markov::binaryStoreSize();
}


void Hidden_semi_markov::restoreGuts(RWvistream &is)

{
  Semi_markov::restoreGuts(is);
}


void Hidden_semi_markov::restoreGuts(RWFile &file)

{
  Semi_markov::restoreGuts(file);
}


void Hidden_semi_markov::saveGuts(RWvostream &os) const

{
  Semi_markov::saveGuts(os);
}


void Hidden_semi_markov::saveGuts(RWFile &file) const

{
  Semi_markov::saveGuts(file);
} */
