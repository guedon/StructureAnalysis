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
#include "stat_tools.h"
#include "distribution.h"
#include "stat_label.h"

using namespace std;


extern char* label(const char *file_name);
extern bool cumul_matching_plot_print(const char *path , int nb_cumul , int *offset ,
                                      int *nb_value , double **cumul);



/*--------------------------------------------------------------*
 *
 *  Initialisation des parametres d'une loi.
 *
 *  arguments : bornes inferieure et superieure, parametre, probabilite.
 *
 *--------------------------------------------------------------*/

void Parametric::init(int iinf_bound , int isup_bound , double iparameter , double iprobability)

{
  inf_bound = iinf_bound;
  sup_bound = isup_bound;
  parameter = iparameter;
  probability = iprobability;
}


/*--------------------------------------------------------------*
 *
 *  Initialisation de l'identificateur et des parametres d'une loi.
 *
 *  arguments : identificateur, bornes inferieure et superieure,
 *              parametre, probabilite.
 *
 *--------------------------------------------------------------*/

void Parametric::init(int iident , int iinf_bound , int isup_bound ,
                      double iparameter , double iprobability)

{
  ident = iident;

  inf_bound = iinf_bound;
  sup_bound = isup_bound;
  parameter = iparameter;
  probability = iprobability;
}


/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe Parametric.
 *
 *  arguments : nombre de valeurs, identificateur, bornes inferieure et
 *              superieure, parametre, probabilite.
 *
 *--------------------------------------------------------------*/

Parametric::Parametric(int inb_value , int iident , int iinf_bound ,
                       int isup_bound , double iparameter , double iprobability)
:Distribution(inb_value)

{
  ident = iident;

  inf_bound = iinf_bound;
  sup_bound = isup_bound;
  parameter = iparameter;
  probability = iprobability;
}


/*--------------------------------------------------------------*
 *
 *  Constructeur de la classe Parametric.
 *
 *  arguments : identificateur, bornes inferieure et superieure,
 *              parametre, probabilite, seuil sur la fonction de repartition.
 *
 *--------------------------------------------------------------*/

Parametric::Parametric(int iident , int iinf_bound , int isup_bound ,
                       double iparameter , double iprobability , double cumul_threshold)

{
  ident = iident;

  inf_bound = iinf_bound;
  sup_bound = isup_bound;
  parameter = iparameter;
  probability = iprobability;

  nb_value = 0;

  if ((ident == BINOMIAL) || (ident == UNIFORM)) {
    nb_value = sup_bound + 1;
  }
  else {
    if ((ident == POISSON) || (ident == NEGATIVE_BINOMIAL)) {
      nb_value = (int)round(inf_bound + (parametric_mean_computation() - inf_bound +
                                         sqrt(parametric_variance_computation())) * 20.);
      if (nb_value == inf_bound) {
        nb_value++;
      }
    }
  }

  Distribution::init(nb_value);

  computation(1 , cumul_threshold);
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet Parametric a partir d'un objet Distribution.
 *
 *  arguments : reference sur un objet Distribution, nombre de valeurs allouees.
 *
 *--------------------------------------------------------------*/

Parametric::Parametric(const Distribution &dist , int ialloc_nb_value)
:Distribution(dist , 'c' , ialloc_nb_value)

{
  ident = NONPARAMETRIC;

  inf_bound = I_DEFAULT;
  sup_bound = I_DEFAULT;
  parameter = D_DEFAULT;
  probability = D_DEFAULT;
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet Parametric a partir d'un objet Distribution
 *  avec changement d'echelle.
 *
 *  arguments : reference sur un objet Distribution, facteur d'echelle.
 *
 *--------------------------------------------------------------*/

Parametric::Parametric(const Distribution &dist , double scale)
:Distribution(dist , scale)

{
  ident = NONPARAMETRIC;

  inf_bound = I_DEFAULT;
  sup_bound = I_DEFAULT;
  parameter = D_DEFAULT;
  probability = D_DEFAULT;
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet Parametric a partir d'un objet Histogram.
 *
 *  argument : reference sur un objet Histogram.
 *
 *--------------------------------------------------------------*/

Parametric::Parametric(const Histogram &histo)
:Distribution(histo)

{
  ident = NONPARAMETRIC;

  inf_bound = I_DEFAULT;
  sup_bound = I_DEFAULT;
  parameter = D_DEFAULT;
  probability = D_DEFAULT;
}


/*--------------------------------------------------------------*
 *
 *  Copie d'un objet Parametric.
 *
 *  argument : reference sur un objet Parametric.
 *
 *--------------------------------------------------------------*/

void Parametric::copy(const Parametric &dist)

{
  ident = dist.ident;

  inf_bound = dist.inf_bound;
  sup_bound = dist.sup_bound;
  parameter = dist.parameter;
  probability = dist.probability;
}


/*--------------------------------------------------------------*
 *
 *  Constructeur par copie de la classe Parametric.
 *
 *  arguments : reference sur un objet Parametric, type de transformation
 *              ('c' : copie , 'n' : copie avec renormalisation),
 *              nombre de valeurs allouees.
 *
 *--------------------------------------------------------------*/

Parametric::Parametric(const Parametric &dist , char transform , int ialloc_nb_value)
:Distribution(dist , transform , ialloc_nb_value)

{
  copy(dist);
}


/*--------------------------------------------------------------*
 *
 *  Operateur d'assignement de la classe Parametric.
 *
 *  argument : reference sur un objet Parametric.
 *
 *--------------------------------------------------------------*/

Parametric& Parametric::operator=(const Parametric &dist)

{
  if (&dist != this) {
    delete [] mass;
    delete [] cumul;

    Distribution::copy(dist);
    copy(dist);
  }

  return *this;
}


/*--------------------------------------------------------------*
 *
 *  Analyse du format d'un objet Parametric.
 *
 *  arguments : reference sur un objet Format_error, stream,
 *              reference sur l'indice de la ligne lue, identificateur
 *              de la derniere loi dans la liste, seuil sur la fonction
 *              de repartition, borne inferieure minimum.
 *
 *--------------------------------------------------------------*/

Parametric* parametric_parsing(Format_error &error , ifstream &in_file , int &line ,
                               int last_ident , double cumul_threshold , int min_inf_bound)

{
  RWLocaleSnapshot locale("en");
  RWCString buffer , token;
  size_t position;
  bool status = true , lstatus;
  register int i , j;
  int ident = I_DEFAULT;
  long inf_bound , sup_bound = I_DEFAULT;
  double parameter = D_DEFAULT , probability = D_DEFAULT;
  Parametric *dist;


  dist = 0;

  while (buffer.readLine(in_file , false)) {
    line++;

#   ifdef DEBUG
    cout << line << " " << buffer << endl;
#   endif

    position = buffer.first('#');
    if (position != RW_NPOS) {
      buffer.remove(position);
    }
    i = 0;

    RWCTokenizer next(buffer);

    while (!((token = next()).isNull())) {

      // test nom de la loi

      if (i == 0) {
        for (j = BINOMIAL;j <= last_ident;j++) {
          if (token == STAT_distribution_word[j]) {
            ident = j;
            break;
          }
        }

        if (j == last_ident + 1) {
          status = false;
          error.update(STAT_parsing[STATP_DISTRIBUTION_NAME] , line , i + 1);
        }
      }

      // test nom du parametre

      else {
        switch ((i - 1) % 3) {

        case 0 : {
          switch ((i - 1) / 3) {

          // 1er parametre : borne inferieure

          case 0 : {
            if (token != STAT_word[STATW_INF_BOUND]) {
              status = false;
              error.correction_update(STAT_parsing[STATP_PARAMETER_NAME] , STAT_word[STATW_INF_BOUND] , line , i + 1);
            }
            break;
          }

          // 2eme parametre : borne superieure (binomiale , uniforme)
          // ou parametre (Poisson , binomiale negative)

          case 1 : {
            if (((ident == BINOMIAL) || (ident == UNIFORM)) &&
                (token != STAT_word[STATW_SUP_BOUND])) {
              status = false;
              error.correction_update(STAT_parsing[STATP_PARAMETER_NAME] , STAT_word[STATW_SUP_BOUND] , line , i + 1);
            }

            if (((ident == POISSON) || (ident == NEGATIVE_BINOMIAL)) &&
                (token != STAT_word[STATW_PARAMETER])) {
              status = false;
              error.correction_update(STAT_parsing[STATP_PARAMETER_NAME] , STAT_word[STATW_PARAMETER] , line , i + 1);
            }
            break;
          }

          // 3eme parametre : probabilite (binomiale , binomiale negative)

          case 2 : {
            if (((ident == BINOMIAL) || (ident == NEGATIVE_BINOMIAL)) &&
                (token != STAT_word[STATW_PROBABILITY])) {
              status = false;
              error.correction_update(STAT_parsing[STATP_PARAMETER_NAME] , STAT_word[STATW_PROBABILITY] , line , i + 1);
            }
            break;
          }
          }

          break;
        }

        // test separateur

        case 1 : {
          if (token != ":") {
            status = false;
            error.update(STAT_parsing[STATP_SEPARATOR] , line , i + 1);
          }
          break;
        }

        // test valeur du parametre

        case 2 : {
          switch ((i - 1) / 3) {

          // 1er parametre : borne inferieure

          case 0 : {
            lstatus = locale.stringToNum(token , &inf_bound);
            if ((lstatus) && ((inf_bound < min_inf_bound) || (inf_bound > MAX_INF_BOUND))) {
              lstatus = false;
            }
            break;
          }

          // 2eme parametre : borne superieure (binomiale , uniforme)
          // ou parametre (Poisson , binomiale negative)

          case 1 : {
            if ((ident == BINOMIAL) || (ident == UNIFORM)) {
              lstatus = locale.stringToNum(token , &sup_bound);

              if (lstatus) {
                switch (ident) {

                case BINOMIAL : {
                  if ((sup_bound <= inf_bound) || (sup_bound - inf_bound > MAX_DIFF_BOUND)) {
                    lstatus = false;
                  }
                  break;
                }

                case UNIFORM : {
                  if ((sup_bound < inf_bound) || (sup_bound - inf_bound > MAX_DIFF_BOUND)) {
                    lstatus = false;
                  }
                  break;
                }
                }
              }
            }

            if ((ident == POISSON) || (ident == NEGATIVE_BINOMIAL)) {
              lstatus = locale.stringToNum(token , &parameter);
              if ((lstatus) && ((parameter <= 0.) ||
                   ((ident == POISSON) && (parameter > MAX_MEAN)))) {
                lstatus = false;
              }
            }
            break;
          }

          // 3eme parametre : probabilite (binomiale , binomiale negative)

          case 2 : {
            if ((ident == BINOMIAL) || (ident == NEGATIVE_BINOMIAL)) {
              lstatus = locale.stringToNum(token , &probability);

              if (lstatus) {
                switch (ident) {

                case BINOMIAL : {
                  if ((probability < 0.) || (probability > 1.)) {
                    lstatus = false;
                  }
                  break;
                }

                case NEGATIVE_BINOMIAL : {
                  if ((probability <= 0.) || (probability >= 1.)) {
                    lstatus = false;
                  }
                  else if (parameter * (1. - probability) / probability > MAX_MEAN) {
                    lstatus = false;
                  }
                  break;
                }
                }
              }
            }
            break;
          }
          }

          if (!lstatus) {
            status = false;
            error.update(STAT_parsing[STATP_PARAMETER_VALUE] , line , i + 1);
          }
          break;
        }
        }
      }

      i++;
    }

    if (i > 0) {
      if ((((ident == BINOMIAL) || (ident == NEGATIVE_BINOMIAL)) && (i != 10)) ||
          (((ident == POISSON) || (ident == UNIFORM)) && (i != 7))) {
        status = false;
        error.update(STAT_parsing[STATP_FORMAT] , line);
      }

      break;
    }
  }

  if (ident == I_DEFAULT) {
    status = false;
    error.update(STAT_parsing[STATP_FORMAT] , line);
  }

  if (status) {
    dist = new Parametric(ident , inf_bound , sup_bound ,
                          parameter , probability , cumul_threshold);
  }

  return dist;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture des parametres d'une loi.
 *
 *  argument : stream.
 *
 *--------------------------------------------------------------*/

ostream& Parametric::ascii_print(ostream &os) const

{
  os << STAT_distribution_word[ident] << "   ";

  if (inf_bound != I_DEFAULT) {
    os << STAT_word[STATW_INF_BOUND] << " : " << inf_bound << "   ";
  }
  if (sup_bound != I_DEFAULT) {
    os << STAT_word[STATW_SUP_BOUND] << " : " << sup_bound << "   ";
  }
  if (parameter != D_DEFAULT) {
    os << STAT_word[STATW_PARAMETER] << " : " << parameter << "   ";
  }
  if (probability != D_DEFAULT) {
    os << STAT_word[STATW_PROBABILITY] << " : " << probability;
  }
  os << endl;

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture des parametres d'une loi au format tableur.
 *
 *  argument : stream.
 *
 *--------------------------------------------------------------*/

ostream& Parametric::spreadsheet_print(ostream &os) const

{
  os << STAT_distribution_word[ident] << "\t";

  if (inf_bound != I_DEFAULT) {
    os << STAT_word[STATW_INF_BOUND] << "\t" << inf_bound << "\t";
  }
  if (sup_bound != I_DEFAULT) {
    os << STAT_word[STATW_SUP_BOUND] << "\t" << sup_bound << "\t";
  }
  if (parameter != D_DEFAULT) {
    os << STAT_word[STATW_PARAMETER] << "\t" << parameter << "\t";
  }
  if (probability != D_DEFAULT) {
    os << STAT_word[STATW_PROBABILITY] << "\t" << probability;
  }
  os << endl;

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture des parametres d'une loi au format Gnuplot (titre d'une courbe).
 *
 *  argument : stream.
 *
 *--------------------------------------------------------------*/

ostream& Parametric::plot_title_print(ostream &os) const

{
  if (ident != NONPARAMETRIC) {
    os << " " << STAT_distribution_letter[ident] << "(";

    os << inf_bound;
    if (sup_bound != I_DEFAULT) {
      os << "," << sup_bound;
    }
    if (parameter != D_DEFAULT) {
      os << "," << parameter;
    }
    if (probability != D_DEFAULT) {
      os << "," << probability;
    }
    os << ")";
  }

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Visualisation d'une loi parametrique.
 *
 *  arguments : stream, reference sur un objet Parametric.
 *
 *--------------------------------------------------------------*/

ostream& operator<<(ostream &os , const Parametric &dist)

{
  os.precision(5);

  os << endl;
  dist.ascii_print(os);
  dist.Distribution::print(os);

  os.precision(6);

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Fonctions pour la persistance.
 *
 *--------------------------------------------------------------*/

/* RWspace Parametric::binaryStoreSize(int ialloc_nb_value) const

{
  RWspace size = Distribution::binaryStoreSize(ialloc_nb_value) + sizeof(ident) +
                 sizeof(inf_bound) + sizeof(sup_bound) + sizeof(parameter) + sizeof(probability);

  return size;
}


void Parametric::restoreGuts(RWvistream &is)

{
  Distribution::restoreGuts(is);

  is >> ident >> inf_bound >> sup_bound >> parameter >> probability;
}


void Parametric::restoreGuts(RWFile &file)

{
  Distribution::restoreGuts(file);

  file.Read(ident);
  file.Read(inf_bound);
  file.Read(sup_bound);
  file.Read(parameter);
  file.Read(probability);
}


void Parametric::saveGuts(RWvostream &os , int ialloc_nb_value) const

{
  Distribution::saveGuts(os , ialloc_nb_value);

  os << ident << inf_bound << sup_bound << parameter << probability;
}


void Parametric::saveGuts(RWFile &file , int ialloc_nb_value) const

{
  Distribution::saveGuts(file , ialloc_nb_value);

  file.Write(ident);
  file.Write(inf_bound);
  file.Write(sup_bound);
  file.Write(parameter);
  file.Write(probability);
} */


/*--------------------------------------------------------------*
 *
 *  Calcul du nombre de parametres d'une loi.
 *
 *--------------------------------------------------------------*/

int Parametric::nb_parameter_computation()

{
  int bnb_parameter;


  switch (ident) {
  case BINOMIAL :
    bnb_parameter = 3;
    break;
  case POISSON :
    bnb_parameter = 2;
    break;
  case NEGATIVE_BINOMIAL :
    bnb_parameter = 3;
    break;
  case UNIFORM :
    bnb_parameter = 2;
    break;
  default :
    bnb_parameter = 0;
    break;
  }

  return bnb_parameter;
}


/*--------------------------------------------------------------*
 *
 *  Mise a jour du nombre de parametres d'une loi.
 *
 *--------------------------------------------------------------*/

void Parametric::nb_parameter_update()

{
  nb_parameter = nb_parameter_computation();
}


/*--------------------------------------------------------------*
 *
 *  Calcul de la moyenne d'une loi parametrique.
 *
 *--------------------------------------------------------------*/

double Parametric::parametric_mean_computation() const

{
  double parametric_mean;


  switch (ident) {
  case BINOMIAL :
    parametric_mean = inf_bound + (sup_bound - inf_bound) * probability;
    break;
  case POISSON :
    parametric_mean = inf_bound + parameter;
    break;
  case NEGATIVE_BINOMIAL :
    parametric_mean = inf_bound + parameter * (1. - probability) / probability;
    break;
  case UNIFORM :
    parametric_mean = (double)(inf_bound + sup_bound) / 2.;
    break;
  default :
    parametric_mean = mean;
    break;
  }

  return parametric_mean;
}


/*--------------------------------------------------------------*
 *
 *  Calcul de la variance d'une loi parametrique.
 *
 *--------------------------------------------------------------*/

double Parametric::parametric_variance_computation() const

{
  double parametric_variance;


  switch (ident) {
  case BINOMIAL :
    parametric_variance = (sup_bound - inf_bound) * probability * (1. - probability);
    break;
  case POISSON :
    parametric_variance = parameter;
    break;
  case NEGATIVE_BINOMIAL :
    parametric_variance = parameter * (1. - probability) / (probability * probability);
    break;
  case UNIFORM :
    parametric_variance = (double)((sup_bound - inf_bound + 2) *
                          (sup_bound - inf_bound)) / 12.;
    break;
  default :
    parametric_variance = variance;
    break;
  }

  return parametric_variance;
}


/*--------------------------------------------------------------*
 *
 *  Calcul du coefficient d'asymetrie d'une loi parametrique.
 *
 *--------------------------------------------------------------*/

double Parametric::parametric_skewness_computation() const

{
  double parametric_skewness;


  switch (ident) {

  case BINOMIAL : {
    if ((probability > 0.) && (probability < 1.)) {
      parametric_skewness = (1. - 2. * probability) /
                            sqrt((sup_bound - inf_bound) * probability * (1. - probability));
    }
    else {
      parametric_skewness = 0.;
    }
    break;
  }

  case POISSON : {
    parametric_skewness = 1. / sqrt(parameter);
    break;
  }

  case NEGATIVE_BINOMIAL : {
    parametric_skewness = (2. - probability) / sqrt(parameter * (1. - probability));
    break;
  }

  case UNIFORM : {
    parametric_skewness = 0.;
    break;
  }

  default : {
    parametric_skewness = D_INF;
    break;
  }
  }

  return parametric_skewness;
}


/*--------------------------------------------------------------*
 *
 *  Calcul de l'exces d'applatissement d'une loi parametrique:
 *  exces d'applatissement = coefficient d'applatissement - 3..
 *
 *--------------------------------------------------------------*/

double Parametric::parametric_kurtosis_computation() const

{
  double parametric_kurtosis;


  switch (ident) {

  case BINOMIAL : {
    if ((probability > 0.) && (probability < 1.)) {
      parametric_kurtosis = (1. - 6. * probability * (1. - probability)) /
                            ((sup_bound - inf_bound) * probability * (1. - probability));
    }
    else {
      parametric_kurtosis = -D_INF;
    }
    break;
  }

  case POISSON : {
    parametric_kurtosis = 1. / parameter;
    break;
  }

  case NEGATIVE_BINOMIAL : {
    parametric_kurtosis = (probability * probability + 6. * (1. - probability)) /
                          (parameter * (1. - probability));
    break;
  }

  case UNIFORM : {
    parametric_kurtosis = -0.5;
    break;
  }

  default : {
    parametric_kurtosis = -D_INF;
    break;
  }
  }

  return parametric_kurtosis;
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet Parametric_model a partir d'un objet Histogram.
 *
 *  argument : reference sur un objet Histogram.
 *
 *--------------------------------------------------------------*/

Parametric_model::Parametric_model(const Histogram &histo)
:Parametric(histo)

{
  histogram = new Distribution_data(histo);
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet Parametric_model a partir
 *  d'un objet Distribution et d'un objet Histogram.
 *
 *  arguments : reference sur un objet Distribution,
 *              pointeur sur un objet Histogram.
 *
 *--------------------------------------------------------------*/

Parametric_model::Parametric_model(const Distribution &dist , const Histogram *histo)
:Parametric(dist)

{
  if ((histo) && (histo->nb_element > 0)) {
    histogram = new Distribution_data(*histo);
  }
  else {
    histogram = 0;
  }
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet Parametric_model a partir
 *  d'un objet Parametric et d'un objet Histogram.
 *
 *  arguments : reference sur un objet Parametric,
 *              pointeur sur un objet Histogram.
 *
 *--------------------------------------------------------------*/

Parametric_model::Parametric_model(const Parametric &dist , const Histogram *histo)
:Parametric(dist)

{
  if ((histo) && (histo->nb_element > 0)) {
    histogram = new Distribution_data(*histo);
  }
  else {
    histogram = 0;
  }
}


/*--------------------------------------------------------------*
 *
 *  Constructeur par copie de la classe Parametric_model.
 *
 *  arguments : reference sur un objet Parametric_model,
 *              flag copie de l'objet Distribution_data.
 *
 *--------------------------------------------------------------*/

Parametric_model::Parametric_model(const Parametric_model &dist , bool data_flag)
:Parametric(dist)

{
  if ((data_flag) && (dist.histogram)) {
    histogram = new Distribution_data(*(dist.histogram) , false);
  }
  else {
    histogram = 0;
  }
}


/*--------------------------------------------------------------*
 *
 *  Destructeur de la classe Parametric_model.
 *
 *--------------------------------------------------------------*/

Parametric_model::~Parametric_model()

{
  delete histogram;
}


/*--------------------------------------------------------------*
 *
 *  Operateur d'assignement de la classe Parametric_model.
 *
 *  argument : reference sur un objet Parametric_model.
 *
 *--------------------------------------------------------------*/

Parametric_model& Parametric_model::operator=(const Parametric_model &dist)

{
  if (&dist != this) {
    delete histogram;

    delete [] mass;
    delete [] cumul;

    Distribution::copy(dist);
    Parametric::copy(dist);

    if (dist.histogram) {
      histogram = new Distribution_data(*(dist.histogram) , false);
    }
    else {
      histogram = 0;
    }
  }

  return *this;
}


/*--------------------------------------------------------------*
 *
 *  Extraction de la partie "donnees" d'un objet Parametric_model.
 *
 *  argument : reference sur un objet Format_error.
 *
 *--------------------------------------------------------------*/

Distribution_data* Parametric_model::extract_data(Format_error &error) const

{
  Distribution_data *histo;


  error.init();

  if (!histogram) {
    histo = 0;
    error.update(STAT_error[STATR_NON_EXISTING_HISTOGRAM]);
  }

  else {
    histo = new Distribution_data(*histogram);
    histo->distribution = new Parametric_model(*this , false);
  }

  return histo;
}


/*--------------------------------------------------------------*
 *
 *  Construction d'un objet Parametric_model a partir d'un fichier.
 *
 *  arguments : reference sur un objet Format_error, path,
 *              seuil sur la fonction de repartition.
 *
 *--------------------------------------------------------------*/

Parametric_model* parametric_ascii_read(Format_error &error , const char *path ,
                                        double cumul_threshold)

{
  RWCString buffer;
  size_t position;
  bool status;
  int line;
  Parametric *pdist;
  Parametric_model *dist;
  ifstream in_file(path);


  dist = 0;
  error.init();

  if (!in_file) {
    error.update(STAT_error[STATR_FILE_NAME]);
  }

  else {
    status = true;
    line = 0;

    pdist = parametric_parsing(error , in_file , line , UNIFORM , cumul_threshold);

    if (!pdist) {
      status = false;
    }

    while (buffer.readLine(in_file , false)) {
      line++;

#     ifdef DEBUG
      cout << line << " " << buffer << endl;
#     endif

      position = buffer.first('#');
      if (position != RW_NPOS) {
        buffer.remove(position);
      }
      if (!(buffer.isNull())) {
        status = false;
        error.update(STAT_parsing[STATP_FORMAT] , line);
      }
    }

    if (status) {
      dist = new Parametric_model(*pdist);
    }

    delete pdist;
  }

  return dist;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture sur une ligne d'un objet Parametric_model.
 *
 *  argument : stream.
 *
 *--------------------------------------------------------------*/

ostream& Parametric_model::line_write(ostream &os) const

{
  os << STAT_distribution_word[ident];

  if ((mean != D_DEFAULT) && (variance != D_DEFAULT)) {
    os << "   " << STAT_label[STATL_MEAN] << ": " << mean
       << "   " << STAT_label[STATL_VARIANCE] << ": " << variance;
  }

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'une loi parametrique et d'un histogramme.
 *
 *  arguments : stream, pointeur sur un histogramme,
 *              flag niveau de detail, flag fichier.
 *
 *--------------------------------------------------------------*/

ostream& Parametric_model::ascii_write(ostream &os , const Distribution_data *histo ,
                                       bool exhaustive , bool file_flag) const

{
  if (ident == NONPARAMETRIC) {
    file_flag = false;
  }

  ascii_print(os);
  if (complement > 0.) {
    os << STAT_label[STATL_UNPROPER] << " " << STAT_label[STATL_DISTRIBUTION] << " ("
       << STAT_label[STATL_COMPLEMENTARY_PROBABILITY] << ": " << complement << ")" << endl;
  }

  ascii_characteristic_print(os , true , file_flag);

  if (file_flag) {
    os << "# ";
  }
  os << STAT_label[STATL_MEAN_ABSOLUTE_DEVIATION] << ": " << mean_absolute_deviation_computation();
  if (mean > 0.) {
    os << "   " << STAT_label[STATL_CONCENTRATION_COEFF] << ": " << concentration_computation();
  }
  os << endl;

  if (histo) {
    double likelihood , information;
    Test test(CHI2);


    os << "\n";
    if (file_flag) {
      os << "# ";
    }
    os << STAT_label[STATL_HISTOGRAM] << " - ";
    histo->ascii_characteristic_print(os , true , file_flag);

    if (file_flag) {
      os << "# ";
    }
    os << STAT_label[STATL_MEAN_ABSOLUTE_DEVIATION] << ": " << histo->mean_absolute_deviation_computation();
    if (histo->mean > 0.) {
      os << "   " << STAT_label[STATL_CONCENTRATION_COEFF] << ": " << histo->concentration_computation();
    }
    os << endl;

    likelihood = likelihood_computation(*histo);
    information = histo->information_computation();

    os << "\n";
    if (file_flag) {
      os << "# ";
    }
    os << STAT_label[STATL_LIKELIHOOD] << ": " << likelihood << "   ("
       << STAT_label[STATL_NORMALIZED] << ": " << likelihood / histo->nb_element << ")" << endl;

    if (file_flag) {
      os << "# ";
    }
    os << STAT_label[STATL_MAX_LIKELIHOOD] << ": " << information << "   ("
       << STAT_label[STATL_INFORMATION] << ": " << information / histo->nb_element << ")" << endl;

    if (file_flag) {
      os << "# ";
    }
    os << STAT_label[STATL_DEVIANCE] << ": " << 2 * (information - likelihood) << endl;

    chi2_fit(*histo , test);
    os << "\n";
    test.ascii_print(os , file_flag);
  }

  else {
    if (file_flag) {
      os << "# ";
    }
    os << STAT_label[STATL_INFORMATION] << ": " << information_computation() << endl;
  }

  if (exhaustive) {
    os << "\n";
    if (file_flag) {
      os << "# ";
    }
    os << "  ";
    if (histo) {
      os << " | " << STAT_label[STATL_HISTOGRAM];
    }
    os << " | " << STAT_label[STATL_DISTRIBUTION];
    if (histo) {
      os << " | " << STAT_label[STATL_CUMULATIVE] << " " << STAT_label[STATL_HISTOGRAM] << " "
         << STAT_label[STATL_FUNCTION];
    }
    os << " | " << STAT_label[STATL_CUMULATIVE] << " " << STAT_label[STATL_DISTRIBUTION] << " "
       << STAT_label[STATL_FUNCTION] << endl;

    Distribution::ascii_print(os , file_flag , true , false , histo);
  }

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Parametric_model.
 *
 *  arguments : stream, flag niveau de detail.
 *
 *--------------------------------------------------------------*/

ostream& Parametric_model::ascii_write(ostream &os , bool exhaustive) const

{
  return ascii_write(os , histogram , exhaustive , false);
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Parametric_model dans un fichier.
 *
 *  arguments : reference sur un objet Format_error, path,
 *              flag niveau de detail.
 *
 *--------------------------------------------------------------*/

bool Parametric_model::ascii_write(Format_error &error , const char *path ,
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
    ascii_write(out_file , histogram , exhaustive , true);
  }

  return status;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'une loi parametrique et d'un histogramme dans un fichier
 *  au format tableur.
 *
 *  arguments : stream, pointeur sur un histogramme.
 *
 *--------------------------------------------------------------*/

ostream& Parametric_model::spreadsheet_write(ostream &os , const Distribution_data *histo) const

{


  spreadsheet_print(os);
  if (complement > 0.) {
    os << STAT_label[STATL_UNPROPER] << " " << STAT_label[STATL_DISTRIBUTION] << "\t"
       << STAT_label[STATL_COMPLEMENTARY_PROBABILITY] << "\t" << complement << endl;
  }

  spreadsheet_characteristic_print(os , true);

  os << STAT_label[STATL_MEAN_ABSOLUTE_DEVIATION] << "\t" << mean_absolute_deviation_computation();
  if (mean > 0.) {
    os << "\t\t" << STAT_label[STATL_CONCENTRATION_COEFF] << "\t" << concentration_computation();
  }
  os << endl;

  if (histo) {
    double likelihood , information;
    Test test(CHI2);


    os << "\n" << STAT_label[STATL_HISTOGRAM] << "\t";
    histo->spreadsheet_characteristic_print(os , true);

    os << STAT_label[STATL_MEAN_ABSOLUTE_DEVIATION] << "\t" << histo->mean_absolute_deviation_computation();
    if (histo->mean > 0.) {
      os << "\t\t" << STAT_label[STATL_CONCENTRATION_COEFF] << "\t" << histo->concentration_computation();
    }
    os << endl;

    likelihood = likelihood_computation(*histo);
    information = histo->information_computation();

    os << "\n" << STAT_label[STATL_LIKELIHOOD] << "\t" << likelihood << "\t"
       << STAT_label[STATL_NORMALIZED] << "\t" << likelihood / histo->nb_element << endl;
    os << STAT_label[STATL_MAX_LIKELIHOOD] << "\t" << information << "\t"
       << STAT_label[STATL_INFORMATION] << "\t" << information / histo->nb_element << endl;
    os << STAT_label[STATL_DEVIANCE] << "\t" << 2 * (information - likelihood) << endl;

    chi2_fit(*histo , test);
    os << "\n";
    test.spreadsheet_print(os);
  }

  else {
    os << STAT_label[STATL_INFORMATION] << "\t" << information_computation() << endl;
  }

  os << "\n";
  if (histo) {
    os << "\t" << STAT_label[STATL_HISTOGRAM];
  }
  os << "\t" << STAT_label[STATL_DISTRIBUTION];
  if (histo) {
    os << "\t" << STAT_label[STATL_CUMULATIVE] << " " << STAT_label[STATL_HISTOGRAM] << " "
       << STAT_label[STATL_FUNCTION];
  }
  os << "\t" << STAT_label[STATL_CUMULATIVE] << " " << STAT_label[STATL_DISTRIBUTION] << " "
     << STAT_label[STATL_FUNCTION];
  if (mean > 0.) {
    if ((histo) && (histo->mean > 0.)) {
      os << "\t" << STAT_label[STATL_HISTOGRAM] << " " << STAT_label[STATL_CONCENTRATION] << " "
         << STAT_label[STATL_FUNCTION] << "\t" << STAT_label[STATL_DISTRIBUTION] << " ";
    }
    else {
      os << "\t";
    }
    os << STAT_label[STATL_CONCENTRATION] << " " << STAT_label[STATL_FUNCTION];
  }
  os << endl;

  Distribution::spreadsheet_print(os , true , true , false , histo);

  return os;
}


/*--------------------------------------------------------------*
 *
 *  Ecriture d'un objet Parametric_model dans un fichier au format tableur.
 *
 *  arguments : reference sur un objet Format_error, path.
 *
 *--------------------------------------------------------------*/

bool Parametric_model::spreadsheet_write(Format_error &error , const char *path) const

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
    spreadsheet_write(out_file , histogram);
  }

  return status;
}


/*--------------------------------------------------------------*
 *
 *  Sortie Gnuplot d'une loi parametrique et d'un histogramme.
 *
 *  arguments : reference sur un objet Format_error, prefixe des fichiers,
 *              titre des figures, pointeur sur un histogramme.
 *
 *--------------------------------------------------------------*/

bool Parametric_model::plot_write(Format_error &error , const char *prefix , const char *title ,
                                  const Distribution_data *histo) const

{
  bool status;


  if (histo) {
    register int i;
    int plot_nb_value , *poffset , *pnb_value;
    double scale , **pcumul , **pconcentration;
    ostringstream *data_file_name;


    error.init();

    // ecriture des fichiers de donnees

    data_file_name = new ostringstream[3];

    poffset = new int[2];
    pnb_value = new int[2];
    pcumul = new double*[2];

    pconcentration = new double*[2];
    pconcentration[1] = 0;

    data_file_name[0] << prefix << 0 << ".dat";

    poffset[0] = histo->offset;
    pnb_value[0] = histo->nb_value;

    // calcul des fonctions de repartition et de concentration de l'histogramme

    scale = histo->nb_element / (1. - complement);
    pcumul[0] = histo->cumul_computation(scale);
    pconcentration[0] = histo->concentration_function_computation(scale);

    status = histo->plot_print((data_file_name[0].str()).c_str() , pcumul[0] , pconcentration[0]);

    if (status) {
      data_file_name[1] << prefix << 1 << ".dat";

      poffset[1] = offset;
      pnb_value[1] = nb_value;
      plot_nb_value = plot_nb_value_computation(histo);
      pcumul[1] = cumul;

      // calcul de la fonction de concentration

      pconcentration[1] = concentration_function_computation();
      plot_print((data_file_name[1].str()).c_str() , pconcentration[1] , scale);

      if ((variance > 0.) && (histo->variance > 0.)) {
        data_file_name[2] << prefix << 2 << ".dat";
        cumul_matching_plot_print((data_file_name[2].str()).c_str() , 2 , poffset , pnb_value , pcumul);
      }
    }

    delete [] poffset;
    delete [] pnb_value;

    delete [] pcumul[0];
    delete [] pcumul;

    delete [] pconcentration[0];
    delete [] pconcentration[1];
    delete [] pconcentration;

    if (status) {

      // ecriture du fichier de commandes et du fichier d'impression

      for (i = 0;i < 2;i++) {
        ostringstream file_name[2];

        switch (i) {
        case 0 :
          file_name[0] << prefix << ".plot";
          break;
        case 1 :
          file_name[0] << prefix << ".print";
          break;
        }

        ofstream out_file((file_name[0].str()).c_str());

        if (i == 1) {
          out_file << "set terminal postscript" << endl;
          file_name[1] << label(prefix) << ".ps";
          out_file << "set output \"" << file_name[1].str() << "\"\n\n";
        }

        out_file << "set border 15 lw 0\n" << "set tics out\n" << "set xtics nomirror\n"
                 << "set title";
        if (title) {
          out_file << " \"" << title << "\"";
        }
        out_file << "\n\n";

        // ajustement

        if (plot_nb_value - 1 < TIC_THRESHOLD) {
          out_file << "set xtics 0,1" << endl;
        }

        out_file << "plot [0:" << MAX(plot_nb_value , 2) - 1 << "] [0:"
                 << (int)(MAX(histo->max , max * scale) * YSCALE) + 1
                 << "] \"" << label((data_file_name[0].str()).c_str()) << "\" using 1:3 title \""
                 << STAT_label[STATL_HISTOGRAM] << "\" with impulses,\\" << endl;
        out_file << "\"" << label((data_file_name[1].str()).c_str()) << "\" using 1:2 title \""
                 << STAT_label[STATL_DISTRIBUTION];
        plot_title_print(out_file);
        out_file << "\" with linespoints" << endl;

        if (plot_nb_value - 1 < TIC_THRESHOLD) {
          out_file << "set xtics autofreq" << endl;
        }

        if (variance > 0.) {

          // fonctions de repartition

          if (i == 0) {
            out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
          }
          out_file << endl;

          if (plot_nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics 0,1" << endl;
          }

          out_file << "plot [0:" << plot_nb_value - 1 << "] [0:" << 1. - complement << "] ";
          if (histo->variance > 0.) {
            out_file << "\"" << label((data_file_name[0].str()).c_str()) << "\" using 1:5 title \""
                     << STAT_label[STATL_CUMULATIVE] << " " << STAT_label[STATL_HISTOGRAM] << " "
                     << STAT_label[STATL_FUNCTION] << "\" with linespoints,\\" << endl;
          }
          out_file << "\"" << label((data_file_name[1].str()).c_str()) << "\" using 1:3 title \""
                   << STAT_label[STATL_CUMULATIVE] << " " << STAT_label[STATL_DISTRIBUTION] << " "
                   << STAT_label[STATL_FUNCTION] << "\" with linespoints" << endl;

          if (plot_nb_value - 1 < TIC_THRESHOLD) {
            out_file << "set xtics autofreq" << endl;
          }

          if (histo->variance > 0.) {

            // mise en correspondance des fonctions de repartition en prenant
            // comme reference celle de la loi

            if (i == 0) {
              out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
            }
            out_file << endl;

            out_file << "set title";
            out_file << " \"";
            if (title) {
              out_file << title << " - ";
            }
            out_file << STAT_label[STATL_CUMULATIVE] << " " << STAT_label[STATL_DISTRIBUTION]
                     << " " << STAT_label[STATL_FUNCTION] << " " << STAT_label[STATL_MATCHING] << "\"\n\n";

            out_file << "set grid\n" << "set xtics 0,0.1\n" << "set ytics 0,0.1" << endl;

            out_file << "plot [0:" << 1. - complement << "] [0:" << 1. - complement << "] \""
                     << label((data_file_name[2].str()).c_str()) << "\" using 2:1 title \""
                     << STAT_label[STATL_CUMULATIVE] << " " << STAT_label[STATL_HISTOGRAM] << " "
                     << STAT_label[STATL_FUNCTION] << "\" with linespoints,\\" << endl;
            out_file << "\"" << label((data_file_name[2].str()).c_str()) << "\" using 2:2 title \""
                     << STAT_label[STATL_CUMULATIVE] << " " << STAT_label[STATL_DISTRIBUTION] << " "
                     << STAT_label[STATL_FUNCTION] << "\" with linespoints" << endl;

            out_file << "unset grid\n" << "set xtics autofreq\n" << "set ytics autofreq" << endl;
          }

          // courbes de concentration

          if (i == 0) {
            out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
          }
          out_file << endl;

          out_file << "set title";
          if (title) {
            out_file << " \"" << title << "\"";
          }
          out_file << "\n\n";

          out_file << "set grid\n" << "set xtics 0,0.1\n" << "set ytics 0,0.1" << endl;

          out_file << "plot [0:" << 1. - complement << "] [0:" << 1. - complement << "] ";
          if (histo->variance > 0.) {
            out_file << "\"" << label((data_file_name[0].str()).c_str()) << "\" using 5:6 title \""
                     << STAT_label[STATL_HISTOGRAM] << " " << STAT_label[STATL_CONCENTRATION] << " "
                     << STAT_label[STATL_CURVE] << "\" with linespoints,\\" << endl;
          }
          out_file << "\"" << label((data_file_name[1].str()).c_str()) << "\" using 3:4 title \""
                   << STAT_label[STATL_DISTRIBUTION] << " " << STAT_label[STATL_CONCENTRATION] << " "
                   << STAT_label[STATL_CURVE] << "\" with linespoints,\\" << endl;
          out_file << "\"" << label((data_file_name[1].str()).c_str()) << "\" using 3:3 notitle with lines" << endl;

          out_file << "unset grid\n" << "set xtics autofreq\n" << "set ytics autofreq" << endl;
        }

        if (i == 1) {
          out_file << "\nset terminal x11" << endl;
        }

        out_file << "\npause 0 \"" << STAT_label[STATL_END] << "\"" << endl;
      }
    }

    delete [] data_file_name;

    if (!status) {
      error.update(STAT_error[STATR_FILE_PREFIX]);
    }
  }

  else {
    const Distribution *dist[1];

    dist[0] = this;
    status = ::plot_write(error , prefix , 1 , dist , title);
  }

  return status;
}


/*--------------------------------------------------------------*
 *
 *  Sortie Gnuplot d'un objet Parametric_model.
 *
 *  arguments : reference sur un objet Format_error, prefixe des fichiers,
 *              titre des figures.
 *
 *--------------------------------------------------------------*/

bool Parametric_model::plot_write(Format_error &error , const char *prefix ,
                                  const char *title) const

{
  return plot_write(error , prefix , title , histogram);
}


/*--------------------------------------------------------------*
 *
 *  Fonctions pour la persistance.
 *
 *--------------------------------------------------------------*/

/* RWDEFINE_COLLECTABLE(Parametric_model , STATI_PARAMETRIC_MODEL);


RWspace Parametric_model::binaryStoreSize() const

{
  RWspace size = Parametric::binaryStoreSize();
  if (histogram) {
    size += histogram->recursiveStoreSize();
  }

  return size;
}


void Parametric_model::restoreGuts(RWvistream &is)

{
  delete histogram;

  Parametric::restoreGuts(is);

  is >> histogram;
  if (histogram == RWnilCollectable) {
    histogram = 0;
  }
}


void Parametric_model::restoreGuts(RWFile &file)

{
  delete histogram;

  Parametric::restoreGuts(file);

  file >> histogram;
  if (histogram == RWnilCollectable) {
    histogram = 0;
  }
}


void Parametric_model::saveGuts(RWvostream &os) const

{
  Parametric::saveGuts(os);

  if (histogram) {
    os << histogram;
  }
  else {
    os << RWnilCollectable;
  }
}


void Parametric_model::saveGuts(RWFile &file) const

{
  Parametric::saveGuts(file);

  if (histogram) {
    file << histogram;
  }
  else {
    file << RWnilCollectable;
  }
} */
