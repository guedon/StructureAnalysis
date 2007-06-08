/* -*-c++-*-
 *  ----------------------------------------------------------------------------
 *
 *       AMAPmod: Exploring and Modeling Plant Architecture
 *
 *       Copyright 1995-2002 UMR Cirad/Inra Modelisation des Plantes
 *
 *       File author(s): J.-B. Durand (jean-baptiste.durand@cirad.fr)
 *
 *       $Source: /usr/cvsmaster/AMAPmod/src/STAT_TREES/src/hmt_algorithms.cpp,v $
 *       $Id: hmt_algorithms.cpp 3193 2007-05-29 10:03:19Z dufourko $
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
 *       MERCHANTABILITY or FITNESS for A PARTICULAR PURPOSE. See the
 *       GNU General Public License for more details.
 *
 *       You should have received a copy of the GNU General Public
 *       License along with this program; see the file COPYING. If not,
 *       write to the Free Software Foundation, Inc., 59
 *       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ----------------------------------------------------------------------------
 */

#include "tree/tree_simple.h"
#include "tree/tree_traits.h"
#include "tree/basic_visitors.h"
#include "stat_tool/stat_tools.h"
#include "stat_tool/stat_label.h"
#include "stat_tool/curves.h"
#include "stat_tool/markovian.h"
#include "stat_tool/distribution_reestimation.h"

#include "sequence_analysis/sequences.h"

#include "int_fl_containers.h"
#include "generic_typed_edge_tree.h"
#include "typed_edge_trees.h"
#include "hidden_markov_tree.h"
#include "hidden_markov_out_tree.h"
#include "tree_labels.h"

// #include "tool/rw_tokenizer.h"
// #include "tool/rw_cstring.h"
// #include "tool/rw_locale.h"

#include <sstream>
#include <iomanip>

extern int cumul_method(int nb_value, const double *cumul, double scale= 1.);
extern char* label(const char*);

using namespace Stat_trees;

/*****************************************************************
 *
 *  Compute the likelihood of the model parameter for a given set
 *  of observed trees and a tree identifier
 *
 **/

double Hidden_markov_out_tree::likelihood_computation(const Trees& otrees,
                                                      int index) const
{
   register int t, j;
   double likelihood, entropy1;
   Hidden_markov_tree_data::int_array iindividual;
   double_array_3d state_marginal= NULL, output_cond= NULL,
                   upward_prob= NULL, upward_parent_prob= NULL,
                   state_entropy= NULL;
   Format_error error;
   Trees *sotrees= NULL, selected;

   assert(index < otrees.get_nb_trees());
   if (index == I_DEFAULT)
      sotrees= new Trees(otrees);
   else
   {
      iindividual= new int[1];
      iindividual[0]= index;
      sotrees= otrees.select_individual(error, 1, iindividual, true);
      if (sotrees == NULL)
         return D_INF;
   }

   // do not use index now: just one individual is selected
   state_marginal_distribution(*sotrees, state_marginal);
   output_conditional_distribution(*sotrees, output_cond);

   likelihood= upward_step(*sotrees, upward_prob, upward_parent_prob,
                           state_entropy, state_marginal, output_cond,
                           entropy1);

   for(t= 0; t < sotrees->get_nb_trees(); t++)
      if ((index == I_DEFAULT) || (t == index))
      {
         for(j= 0; j < nb_state; j++)
         {
            delete [] state_marginal[t][j];
            state_marginal[t][j]= NULL;
            delete [] output_cond[t][j];
            output_cond[t][j]= NULL;
            delete [] upward_prob[t][j];
            upward_prob[t][j]= NULL;
            delete [] upward_parent_prob[t][j];
            upward_parent_prob[t][j]= NULL;
            delete [] state_entropy[t][j];
            state_entropy[t][j]= NULL;
         }

         delete [] state_marginal[t];
         state_marginal[t]= NULL;
         delete [] output_cond[t];
         output_cond[t]= NULL;
         delete [] upward_prob[t];
         upward_prob[t]= NULL;
         delete [] upward_parent_prob[t];
         upward_parent_prob[t]= NULL;
         delete [] state_entropy[t];
         state_entropy[t]= NULL;
      }

   delete [] state_marginal;
   state_marginal= NULL;
   delete [] output_cond;
   output_cond= NULL;
   delete [] upward_prob;
   upward_prob= NULL;
   delete [] upward_parent_prob;
   upward_parent_prob= NULL;
   delete [] state_entropy;
   state_entropy= NULL;

   delete sotrees;
   sotrees= NULL;
   if (index != I_DEFAULT)
   {
      delete [] iindividual;
      iindividual= NULL;
   }

   return likelihood;
}

double Hidden_markov_out_tree::likelihood_computation(const Hidden_markov_tree_data& otrees) const
{
   register int t, j;
   double likelihood, entropy1;
   double_array_3d state_marginal= NULL, output_cond= NULL,
                   upward_prob= NULL, upward_parent_prob= NULL,
                   state_entropy= NULL;

   state_marginal_distribution(otrees, state_marginal);
   output_conditional_distribution(otrees, output_cond);

   likelihood= upward_step(otrees, upward_prob, upward_parent_prob,
                           state_entropy, state_marginal, output_cond, entropy1);

   for(t= 0; t < otrees.get_nb_trees(); t++)
   {
      for(j= 0; j < nb_state; j++)
      {
         delete [] state_marginal[t][j];
         state_marginal[t][j]= NULL;
         delete [] output_cond[t][j];
         output_cond[t][j]= NULL;
         delete [] upward_prob[t][j];
         upward_prob[t][j]= NULL;
         delete [] upward_parent_prob[t][j];
         upward_parent_prob[t][j]= NULL;
         delete [] state_entropy[t][j];
         state_entropy[t][j]= NULL;
      }

      delete [] state_marginal[t];
      state_marginal[t]= NULL;
      delete [] output_cond[t];
      output_cond[t]= NULL;
      delete [] upward_prob[t];
      upward_prob[t]= NULL;
      delete [] upward_parent_prob[t];
      upward_parent_prob[t]= NULL;
      delete [] state_entropy[t];
      state_entropy[t]= NULL;
   }

   delete [] state_marginal;
   state_marginal= NULL;
   delete [] output_cond;
   output_cond= NULL;
   delete [] upward_prob;
   upward_prob= NULL;
   delete [] upward_parent_prob;
   upward_parent_prob= NULL;
   delete [] state_entropy;
   state_entropy= NULL;

   return likelihood;
}

/*****************************************************************
 *
 *  Computation of the optimal state trees for a hidden Markov
 *  out tree, using a Format_error object, (Default_)observed_trees,
 *  the type of restoration algorithm (FORWARD_BACKWARD or VITERBI),
 *  a flag on the computation of the characteristic distributions
 *  and the index of the considered tree (I_DEFAULT for all trees).
 *
 **/

Hidden_markov_tree_data* Hidden_markov_out_tree::state_tree_computation(Format_error& error,
                                                                        const Trees& otrees,
                                                                        int algorithm,
                                                                        bool characteristic_flag,
                                                                        int index,
                                                                        int entropy_algo) const
{
   bool status= true;
   register int var;
   int nb_value, nindex= 0;
   double max_marginal_entropy, entropy;
   int *identifier= NULL;
   std::deque<int> *path= NULL;
   Hidden_markov_tree *hmarkovt= NULL;
   Hidden_markov_tree_data *res_trees= NULL;
   Trees *selected= NULL;

   error.init();

   if ((_nb_ioutput_process != otrees._nb_integral)
        || (_nb_doutput_process != otrees._nb_float))
   {
      status= false;
      error.update(STAT_TREES_error[STATR_NB_OUTPUT_PROCESS]);
   }
   else
   {
      for(var= 0; var < _nb_ioutput_process; var++)
      {
         if (npprocess[var+1] != NULL)
            nb_value= npprocess[var+1]->nb_value;
         else
            nb_value= piprocess[var+1]->nb_value;

         if (nb_value < otrees.characteristics[var]->get_nb_values())
         {
            status= false;
            ostringstream error_message;
            error_message << STAT_label[STATL_OUTPUT_PROCESS] << " " << var+1 << ": "
                          << STAT_TREES_error[STATR_NB_OUTPUT];
            error.update((error_message.str()).c_str());
         }
      }
   }

   if (status)
   {
      assert(index < otrees.get_nb_trees());
      if (index == I_DEFAULT)
      {
         nindex= index;
         res_trees= new Hidden_markov_tree_data(otrees);
      }
      else
      {
         nindex= 0;
         identifier= new int[1];
         identifier[0]= index;
         selected= otrees.select_individual(error, 1, identifier, true);
         if (selected != NULL)
         {
            res_trees= new Hidden_markov_tree_data(*selected);
            delete selected;
            selected= NULL;
            delete [] identifier;
            identifier= NULL;
         }
         else
         {
            res_trees= NULL;
            return res_trees;
         }
      }

      res_trees->markov= new Hidden_markov_out_tree(*this, false, false);

      hmarkovt= new Hidden_markov_out_tree(*this, false, false);

      switch (algorithm)
      {
         case FORWARD_BACKWARD :
         {
            res_trees->build_state_trees();
            res_trees->hidden_likelihood= hmarkovt->upward_downward(*res_trees,
                                                                    max_marginal_entropy,
                                                                    entropy,
                                                                    res_trees->likelihood,
                                                                    path,
                                                                    nindex, NULL,
                                                                    'a', 0);
            if (path != NULL)
            {
               delete path;
               path= NULL;
            }
            break;
         }
         case VITERBI :
         {
            hmarkovt->create_cumul();
            hmarkovt->log_computation();

            res_trees->build_state_trees();
            res_trees->hidden_likelihood= hmarkovt->viterbi(*res_trees, nindex);
            break;
         }
      }
      delete hmarkovt;
      hmarkovt= NULL;

      // computation of the tree characteristics
      // and of the model characteristic distributions
/*      if (res_trees->hidden_likelihood == D_INF)
      {
         delete res_trees;
         res_trees= NULL;
         error.update(STAT_TREES_error[STATR_STATE_TREE_COMPUTATION_FAILURE]);
      }

      else
      {

*/
      res_trees->_nb_states= nb_state; // -1 ?

      // res_trees->chain_data= new Chain_data(*res_trees, 0, 1, this);

      res_trees->chain_data= new Chain_data(type, nb_state, nb_state);
      res_trees->build_characteristics();
      res_trees->build_size_histogram();
      res_trees->build_nb_children_histogram();
      res_trees->build_observation_histogram();

      if (characteristic_flag)
         res_trees->markov->characteristic_computation(*res_trees, true);
   // }
   }
   return res_trees;
}

/*****************************************************************
 *
 *  Compute the state profiles of a Hidden_markov_tree
 *  using a Format_error, a given tree referred to by its identifier,
 *  including the suboptimal state trees computed by simulation
 *  or generalized Viterbi algorithm, using nb_state_trees suboptimal trees,
 *  and return various information contained in messages
 *
 **/

bool Hidden_markov_out_tree::state_profile(Format_error& error,
                                           const Hidden_markov_tree_data& trees,
                                           int index,
                                           Hidden_markov_tree_data*& smoothed_state_tree,
                                           Hidden_markov_tree_data*& nstate_trees,
                                           Hidden_markov_tree_data*& viterbi_upward_downward,
                                           Hidden_markov_tree_data*& generalized_restoration,
                                           std::vector<ostringstream*>& messages,
                                           int state_tree,
                                           unsigned int nb_state_trees,
                                           int entropy_algo) const
{
   bool status= true;
   long double nb_possible_state_trees;
   double likelihood, hidden_likelihood, state_likelihood;
          // ambiguity;
   ostringstream *m;
   std::deque<int> *path= NULL;
   Hidden_markov_tree_data *smoothed_tree= NULL, *cptrees= NULL;
                           // state tree restored by smoothing with no state variable
   Hidden_markov_out_tree *hmarkov= NULL;

   error.init();

   if ((index >= trees._nb_trees) || (index < 0))
   {
      status= false;
      error.update(STAT_TREES_error[STATR_TREE_IDENTIFIER]);
   }

   if (nb_state_trees < 2)
   {
     status= false;
     error.update(STAT_TREES_error[STATR_NB_STATE_TREES]);
   }

   if (status)
   {
      hmarkov= new Hidden_markov_out_tree(*this, false, false);
      cptrees= new Hidden_markov_tree_data(trees, false);
      cptrees->markov= hmarkov;

      cptrees->build_state_trees();
      hmarkov->create_cumul();
      hmarkov->log_computation();
      if (smoothed_tree == NULL)
      {
         smoothed_state_tree=
            cptrees->get_state_smoothed_hidden_markov_tree_data(index, FORWARD_BACKWARD,
                                                                entropy_algo);
         // add restored states, smoothed probabilities and entropy profiles as variables

         // likelihood of the given observed tree
         likelihood= hmarkov->likelihood_computation(*cptrees, index);
         // likelihood of the state and observed trees
         hidden_likelihood= smoothed_state_tree->hidden_likelihood; // hmarkov->viterbi(*cptrees, index);

#        ifdef MESSAGE
         nb_possible_state_trees= hmarkov->nb_state_trees(*cptrees, index);
         if ((nb_possible_state_trees <= UINT_MAX) && (nb_possible_state_trees >= 1))
            nb_state_trees= min((unsigned int)nb_possible_state_trees, nb_state_trees);
#        endif

         m= new ostringstream;
         *m << "\n" << STAT_TREES_label[STATL_POSTERIOR_STATE_PROBABILITY] << "\n\n";

         messages.push_back(m);
         m= new ostringstream;
         *m << "\n" << STAT_label[STATL_LIKELIHOOD] << ": " << likelihood
            << "\n" << STAT_TREES_label[STATL_STATE_TREE_LIKELIHOOD] << ": " << hidden_likelihood
            << "   (" << exp(hidden_likelihood-likelihood) << ")" << endl;

#        ifdef MESSAGE
         *m << "\n" << STAT_TREES_label[STATL_NB_STATE_TREE] << ": " << nb_possible_state_trees << endl;
#        endif
         messages.push_back(m);
         // delete smoothed_tree;
         smoothed_tree= NULL;
         m= new ostringstream;
         *m << ""; // shows the separation between results
         messages.push_back(m);
      }
      else
         status= false;
      // viterbi upward_downward
      viterbi_upward_downward= hmarkov->viterbi_upward_downward(*cptrees,
                                                                messages,
                                                                likelihood,
                                                                state_likelihood,
                                                                path,
                                                                index, NULL,
                                                                'a', 0);
      if (path != NULL)
      {
         delete path;
         path= NULL;
      }
      if (viterbi_upward_downward != NULL)
      {
         m= new ostringstream;
         *m << ""; // shows the separation between results
         messages.push_back(m);
      }

      generalized_restoration= hmarkov->generalized_viterbi(*cptrees,
                                                            messages,
                                                            nb_state_trees,
                                                            likelihood,
                                                            index);
      hmarkov->remove_cumul();
      delete hmarkov;
      hmarkov= NULL;
      // delete cptrees;
      // cptrees= NULL;
   }
   return status;
}

/*****************************************************************
 *
 *  Compute the state, entropy and Viterbi profiles of a Hidden_markov_out_tree
 *  using a Format_error, a file name prefix, Hidden_markov_tree_data,
 *  the tree identifier, a vertex identifier that defines the considered tree path
 *  and the figure titles.
 *
 **/

bool Hidden_markov_out_tree::tree_state_profile_plot_write(Format_error &error,
                                                           const char *prefix,
                                                           const Hidden_markov_tree_data& otrees,
                                                           int identifier,
                                                           int vertex,
                                                           const char *title,
                                                           int algorithm) const
{
   bool status= true;
   register int i, j;
   // int index;
   double likelihood, max_marginal_entropy, entropy, state_likelihood;
   std::deque<int> *path= NULL;
   Hidden_markov_tree *hmarkov= NULL;
   Hidden_markov_tree_data *trees= NULL, *tree_profiles= NULL;
   ostringstream data_file_name[2];
   std::vector<ostringstream*> msg;
   ostringstream *m= NULL;
   ofstream *data_out_file;

   error.init();

   if ((identifier < 0 ) || (identifier >= otrees._nb_trees))
   {
     status = false;
     error.update(STAT_TREES_error[STATR_TREE_IDENTIFIER]);
   }

   if (status)
   {
      // write data files

      data_file_name[0] << prefix << 0 << ".dat";
      data_out_file= new ofstream((data_file_name[0].str()).c_str());

      if (data_out_file == NULL)
      {
        status= false;
        error.update(STAT_error[STATR_FILE_PREFIX]);
      }
      else
      {
         if (otrees._type[0] == INT_VALUE)
         {
            trees= new Hidden_markov_tree_data(otrees, false);
            trees->_type[0]= STATE;
         }
         else
            trees= new Hidden_markov_tree_data(otrees , false);

         state_likelihood= upward_downward(*trees, max_marginal_entropy,
                                           entropy, likelihood, path, identifier, data_out_file,
                                           'g', vertex);

         data_out_file->close();
         delete data_out_file;

         data_file_name[1] << prefix << 1 << ".dat";
         data_out_file = new ofstream((data_file_name[1].str()).c_str());

         hmarkov= new Hidden_markov_out_tree(*this, false);

         hmarkov->create_cumul();
         hmarkov->log_computation();

         tree_profiles= hmarkov->viterbi_upward_downward(*trees, msg, likelihood, state_likelihood,
                                                         path, identifier, data_out_file,
                                                         'g' , vertex);
         delete tree_profiles;
         tree_profiles= NULL;

         while (!msg.empty())
         {
            m= msg.back();
            msg.pop_back();
            delete m;
            m= NULL;
         }

         hmarkov->remove_cumul();
         data_out_file->close();
         delete data_out_file;

         // write command and print files

         for(i= 0; i < 2; i++)
         {
            ostringstream file_name[2];

            switch (i)
            {
            case 0 :
               file_name[0] << prefix << ".plot";
               break;
            case 1 :
               file_name[0] << prefix << ".print";
               break;
            }

            ofstream out_file((file_name[0].str()).c_str());

            if (i == 1)
            {
               out_file << "set terminal postscript" << endl;
               file_name[1] << label(prefix) << ".ps";
               out_file << "set output \"" << file_name[1].str() << "\"\n\n";
            }

            out_file << "set border 15 lw 0\n" << "set tics out\n" << "set xtics nomirror\n"
                     << "set title \"";
            if (title != NULL)
              out_file << title << " - ";

            out_file << STAT_TREES_label[STATL_POSTERIOR_STATE_PROBABILITY] << "\"\n\n";

            if ((int)path->size() - 1 < TIC_THRESHOLD)
                out_file << "set xtics 0,1" << endl;

            out_file << "plot [0:" << path->size() - 1 << "] [0:1] ";
            for(j= 0; j < nb_state; j++)
            {
               out_file << "\"" << label((data_file_name[0].str()).c_str()) << "\" using "
                        << j + 1 << " title \"" << STAT_label[STATL_STATE] << " "
                        << j << "\" with linespoints";
               if (j < nb_state - 1)
                 out_file << ",\\";

               out_file << endl;
            }

            if (i == 0)
               out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;

            out_file << endl;

            out_file << "set title \"";
            if (title != NULL)
               out_file << title << " - ";

            out_file << STAT_TREES_label[STATL_MAX_POSTERIOR_STATE_PROBABILITY] << "\"\n\n";

            out_file << "plot [0:" << path->size() - 1 << "] [0:"
                     << exp(state_likelihood - likelihood) << "] ";
            for(j= 0; j < nb_state; j++)
            {
               out_file << "\"" << label((data_file_name[1].str()).c_str()) << "\" using "
                        << j + 1 << " title \"" << STAT_label[STATL_STATE] << " "
                        << j << "\" with linespoints";
               if (j < nb_state - 1)
                  out_file << ",\\";
               out_file << endl;
            }

            if (i == 0)
              out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;

            out_file << endl;

            out_file << "set title";
            if (title != NULL)
               out_file << " \"" << title << "\"";
            out_file << "\n\n";

            out_file << "plot [0:" << path->size() - 1 << "] [0:" << max_marginal_entropy << "] "
                     << "\"" << label((data_file_name[0].str()).c_str()) << "\" using "
                     << nb_state + 1 << " title \"" << STAT_TREES_label[STATL_CONDITIONAL_ENTROPY]
                     << "\" with linespoints,\\" << endl;
            out_file << "\"" << label((data_file_name[0].str()).c_str()) << "\" using "
                     << nb_state + 2 << " title \"" << STAT_TREES_label[STATL_MARGINAL_ENTROPY]
                     << "\" with linespoints" << endl;

            if (i == 0)
               out_file << "\npause -1 \"" << STAT_label[STATL_HIT_RETURN] << "\"" << endl;
            out_file << endl;

            out_file << "set title";
            if (title != NULL)
               out_file << " \"" << title << "\"";
            out_file << "\n\n";

            out_file << "plot [0:" << path->size() - 1 << "] [0:" << entropy << "] "
                     << "\"" << label((data_file_name[0].str()).c_str()) << "\" using "
                     << nb_state + 3 << " title \"" << STAT_TREES_label[STATL_PARTIAL_STATE_TREE_ENTROPY]
                     << "\" with linespoints" << endl;

            if ((int)path->size() - 1 < TIC_THRESHOLD)
              out_file << "set xtics autofreq" << endl;

            if (i == 1)
              out_file << "\nset terminal x11" << endl;

            out_file << "\npause 0 \"" << STAT_label[STATL_END] << "\"" << endl;
         }

         delete trees;
         trees= NULL;
         delete hmarkov;
         hmarkov= NULL;
         if (path != NULL)
         {
            delete path;
            path= NULL;
         }
      }
   }

   return status;

}

/*****************************************************************
 *
 *  Simulation of a Hidden_markov_out_tree
 *  using a Format_error, a tree size and a children number histogram,
 *  a flag on the counting distribution computation
 *  and a flat on the Kullback-Leibler discrepancy computation
 *
 **/

Hidden_markov_tree_data* Hidden_markov_out_tree::simulation(Format_error& error,
                                                            const Histogram& ihsize,
                                                            const Histogram& ihnb_children,
                                                            bool counting_flag,
                                                            bool divergence_flag) const
{
   typedef Trees::tree_type tree_type;
   typedef generic_visitor<tree_type>::vertex_array vertex_array;

   bool status= true;
   register int t, i, var;
   unsigned int node; // j,
   int cumul_size, cumul_nb_children, parent_state;
   // double **pcumul;
   Hidden_markov_tree_data *res= NULL;
   Hidden_markov_tree *markov;
   Typed_edge_one_int_tree *state_tree;
   tree_type *tree;
   Typed_edge_one_int_tree::value state_v;
   tree_type::value obs_v;
   tree_type::key parent_key;
   generic_visitor<tree_type> *visitor;
   vertex_array va;

   error.init();

   if ((ihsize.nb_element < 1) || (ihsize.nb_element > NB_TREES))
   // nb_element represents the number of observed trees
   {
      status= false;
      error.update(STAT_TREES_error[STATR_NB_TREES]);
   }
   if (ihsize.offset < 2)
   {
      status= false;
      error.update(STAT_TREES_error[STATR_SMALL_TREE_SIZE]);
   }
   if (ihsize.nb_value-1 > MAX_SIZE)
   {
      status= false;
      error.update(STAT_TREES_error[STATR_BIG_TREE_SIZE]);
   }

   if (ihnb_children.nb_element < 1)
   {
      status= false;
      error.update(STAT_TREES_error[STATR_NB_TREES]);
   }
   if (ihnb_children.nb_value-1 > MAX_CHILDREN)
   {
      status= false;
      error.update(STAT_TREES_error[STATR_WIDE_TREE]);
   }

   if (status)
   {
      cumul_size= 0;
      for(i= ihsize.offset; i < ihsize.nb_value; i++)
         cumul_size+= i * ihsize.frequency[i];

      if (cumul_size > CUMUL_SIZE)
      {
         status= false;
         error.update(STAT_TREES_error[STATR_TREE_CUMULATIVE_SIZE]);
      }

      cumul_nb_children= 0;
      for(i= ihnb_children.offset; i < ihnb_children.nb_value; i++)
         cumul_nb_children+= i * ihnb_children.frequency[i];

      if (cumul_nb_children > CUMUL_NB_CHILDREN)
      {
         status= false;
         error.update(STAT_TREES_error[STATR_TREE_CUMULATIVE_CHILDREN]);
      }
   }

   if (status)
   {
      // initializations
      res= new Hidden_markov_tree_data(_nb_ioutput_process, _nb_doutput_process,
                                       ihsize, ihnb_children,
                                       false, true);
      for(var= 0; var < _nb_ioutput_process; var++)
         res->_type[0]= INT_VALUE;

      for(var= 0; var < _nb_doutput_process; var++)
         res->_type[0]= REAL_VALUE;

      res->markov= new Hidden_markov_out_tree(*this, false, false);

      markov= res->markov; // *markov is a Hidden_markov_tree
      markov->create_cumul();
      markov->cumul_computation();
      // N.B. : Chain::create_cumul(), etc.

      obs_v.reset(_nb_ioutput_process, _nb_doutput_process);

      for(t= 0; t < res->get_nb_trees(); t++)
      {
         state_tree= res->state_trees[t];
         tree= res->trees[t];

         state_v.Int()= cumul_method(markov->nb_state, markov->cumul_initial);
         state_tree->put(state_tree->root(), state_v);
         // simulation of the root state

         for(var= 0; var < markov->_nb_ioutput_process; var++)
            if (markov->npprocess[var+1] != NULL)
               obs_v.Int(var)= markov->npprocess[var+1]->observation[state_v.Int()]->simulation();
            else
               obs_v.Int(var)= markov->piprocess[var+1]->observation[state_v.Int()]->simulation();

         for(var= 0; var < markov->_nb_doutput_process; var++)
            obs_v.Double(var)= markov->pdprocess[var]->observation[state_v.Int()]->simulation();

         tree->put(tree->root(), obs_v);
         // simulation of the root observation

         visitor= new generic_visitor<tree_type>;
         traverse_tree(tree->root(), *tree, *visitor);

         va= visitor->get_preorder(*tree);
         delete visitor;

         for(node= 1; node < va.size(); node++)
         // starting from node= 1 skips the root node
         {
            // va(node) is a key
            parent_key= state_tree->parent(va[node]);
            parent_state= (state_tree->get(parent_key)).Int();

            state_v.Int()= cumul_method(markov->nb_state, markov->cumul_transition[parent_state]);
            state_tree->put(va[node], state_v);
            // simulation of current state

            for(var= 0; var < markov->_nb_ioutput_process; var++)
               if (markov->npprocess[var+1] != NULL)
                  obs_v.Int(var)= markov->npprocess[var+1]->observation[state_v.Int()]->simulation();
               else
                  obs_v.Int(var)= markov->piprocess[var+1]->observation[state_v.Int()]->simulation();

            for(var= 0; var < markov->_nb_doutput_process; var++)
               obs_v.Double(var)= markov->pdprocess[var]->observation[state_v.Int()]->simulation();

            tree->put(va[node], obs_v);
         }
      }

      markov->remove_cumul();

      // extraction of the characteristics for the simulated trees

      res->min_max_value_computation();

      // res->chain_data= new Chain_data(*res, 0, 1); // 1 == markov->order
      res->chain_data= new Chain_data(type, 0, 1);
      res->build_characteristics();
      res->build_size_histogram();
      res->build_nb_children_histogram();
      res->_nb_states= nb_state;
      res->build_observation_histogram();
      // res->build_state_characteristics(); // called by build_observation_histogram();

      if (!divergence_flag)
      {
         markov->characteristic_computation(*res, counting_flag);

         // likelihood computation
         res->likelihood= markov->likelihood_computation(*res);
         markov->create_cumul();
         markov->log_computation();

         res->hidden_likelihood= markov->viterbi(*res);

         markov->remove_cumul();
      }
   }
   return res;
}

/*****************************************************************
 *
 *  Simulation of a Hidden_markov_out_tree
 *  using a Format_error, the size and number of children of the trees,
 *  and a flag on the counting distribution computation
 *
 **/

Hidden_markov_tree_data* Hidden_markov_out_tree::simulation(Format_error& error,
                                                            int inb_trees,
                                                            int isize,
                                                            int inb_children_max,
                                                            bool counting_flag) const
{
   bool status= true;
   Hidden_markov_tree_data *res= NULL;

   error.init();

   if ((isize < 1) || (isize > NB_TREES))
   {
      status= false;
      error.update(STAT_TREES_error[STATR_NB_TREES]);
   }
   if (isize < 2)
   {
      status= false;
      error.update(STAT_TREES_error[STATR_SMALL_TREE_SIZE]);
   }
   if (isize > MAX_SIZE)
   {
      status= false;
      error.update(STAT_TREES_error[STATR_BIG_TREE_SIZE]);
   }

   if (inb_children_max < 1)
   {
      status= false;
      error.update(STAT_TREES_error[STATR_NARROW_TREE]);
   }
   if (inb_children_max > MAX_CHILDREN)
   {
      status= false;
      error.update(STAT_TREES_error[STATR_WIDE_TREE]);
   }

  if (status) {
    Histogram hsize(isize+1), hnb_children(inb_children_max+1);

    hsize.nb_element= inb_trees;
    hsize.offset= isize;
    hsize.max= inb_trees;
    hsize.mean= isize;
    hsize.variance= 0.;
    hsize.frequency[isize]= inb_trees;

    hnb_children.nb_element= inb_trees;
    hnb_children.offset= inb_children_max;
    hnb_children.max= inb_trees;
    hnb_children.mean= inb_children_max;
    hnb_children.variance= 0.;
    hnb_children.frequency[inb_children_max]= inb_trees;

    res= simulation(error, hsize, hnb_children, counting_flag);
  }

  return res;
}

/*****************************************************************
 *
 *  Simulation of a Hidden_markov_out_tree
 *  using a Format_error, the number of trees,
 *  (Default_)observed_tres
 *  and a flag on the counting distribution computation
 *
 **/

Hidden_markov_tree_data* Hidden_markov_out_tree::simulation(Format_error& error,
                                                            int inb_trees,
                                                            const Trees& otrees,
                                                            bool counting_flag) const
{
   Histogram *hsize, *hnb_children;
   Hidden_markov_tree_data *res;

   error.init();

   if ((inb_trees < 1) || (inb_trees > NB_TREES))
   {
      res= NULL;
      error.update(STAT_TREES_error[STATR_NB_TREES]);
   }
   else
   {
      hsize= otrees.hsize->frequency_scale(inb_trees);
      hnb_children= otrees.hnb_children->frequency_scale(inb_trees);

      res= simulation(error, *hsize, *hnb_children, counting_flag);
      delete hsize;
      delete hnb_children;
   }
   return res;
}

void Hidden_markov_out_tree::state_no_occurrence_probability(int state, double increment)
{}

void Hidden_markov_out_tree::state_first_occurrence_root_distribution(int state,
                                                                      int min_nb_value,
                                                                      double cumul_threshold)
{}

void Hidden_markov_out_tree::state_first_occurrence_leaves_distribution(int state,
                                                                        int min_nb_value,
                                                                        double cumul_threshold)
{}

void Hidden_markov_out_tree::state_leave_probability(const double * memory,
                                                     int state,
                                                     double increment)
{}

void Hidden_markov_out_tree::state_sojourn_size_distribution(const double * memory,
                                                             int state,
                                                             int min_nb_value,
                                                             double cumul_threshold)
{}

void Hidden_markov_out_tree::state_nb_pattern_mixture(int state, char pattern)
{}

void Hidden_markov_out_tree::output_no_occurrence_probability(int variable,
                                                              int output,
                                                              double increment)
{}

void Hidden_markov_out_tree::output_first_occurrence_root_distribution (int variable,
                                                                       int output,
                                                                       int min_nb_value,
                                                                       double cumul_threshold)
{}

void Hidden_markov_out_tree::output_first_occurrence_leaves_distribution (int variable,
                                                                         int output,
                                                                         int min_nb_value,
                                                                         double cumul_threshold)
{}

void Hidden_markov_out_tree::output_leave_probability(const double * memory,
                                                      int variable,
                                                      int output,
                                                      double increment)
{}

void Hidden_markov_out_tree::output_sojourn_size_distribution(const double * memory,
                                                              int variable,
                                                              int output,
                                                              int min_nb_value,
                                                              double cumul_threshold)
{}

void Hidden_markov_out_tree::output_nb_zones_mixture(int variable, int output)
{}

void Hidden_markov_out_tree::output_nb_occurrences_mixture(int variable, int output)
{}

/*****************************************************************
 *
 *  Computation of the hidden state marginal distributions
 *  for Hidden_markov_out_tree class
 *  using a Hidden_markov_tree_data object
 *  and the index of considered tree
 *
 **/

void Hidden_markov_out_tree::state_marginal_distribution(const Hidden_markov_tree_data& trees,
                                                         double_array_3d& state_marginal,
                                                         int index) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;

   int nb_trees= trees._nb_trees, t, current_size, nb_vertices, nb_children,
       nb_children_max, next_nb_children_max;
   register int i, j;
   double *marginal= new double[nb_state], *next_marginal= new double[nb_state];
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree;
   visitor v;
   vertex_array va;

   // state_marginal[t][j][u] corresponds to the probability that
   // the state variable is equal to j at node u for tree t

   if (state_marginal == NULL)
   {
      state_marginal= new double_array_2d[nb_trees];
      for(t= 0; t < nb_trees; t++)
         state_marginal[t]= NULL;
   }

   for(t= 0; t < nb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         if (state_marginal[t] == NULL)
         {
            state_marginal[t]= new double*[nb_state];
            for(i= 0; i < nb_state; i++)
               state_marginal[t][i]= NULL;
         }
         current_tree= trees.trees[t];
         current_size= current_tree->get_size();
         for(i= 0; i < nb_state; i++)
            if (state_marginal[t][i] == NULL)
               state_marginal[t][i]= new double[current_size];

         nb_vertices= 0; // number of processed vertices of current tree
                         // (also index of current vertex)
         va= v.get_breadthorder(*current_tree);

         for(j= 0; j < nb_state; j++)
         {
            state_marginal[t][j][va[nb_vertices]]= initial[j];
            marginal[j]= initial[j];
         }
         // initialization : at root node, marginal distribution
         // given by "initial" parameter

         nb_children_max= current_tree->get_nb_children(va[nb_vertices]);
         // number of vertices at current depth
         nb_vertices++;
         while (nb_vertices < current_size)
         {
            nb_children= 0; // number of processed vertices at current depth
            next_nb_children_max= 0;
            // number of vertices at next depth

            for(j= 0; j < nb_state; j++)
            {
               next_marginal[j]= transition[0][j] * marginal[0];
               for(i= 1; i < nb_state; i++)
                  next_marginal[j]+= transition[i][j] * marginal[i];
            }
            for(j= 0; j < nb_state; j++)
               marginal[j]= next_marginal[j];

            while (nb_children < nb_children_max)
            {
               next_nb_children_max+= current_tree->get_nb_children(va[nb_vertices]);
               for(j= 0; j < nb_state; j++)
                  state_marginal[t][j][va[nb_vertices]]= marginal[j];

               nb_vertices++;
               nb_children++;
            }
            nb_children_max= next_nb_children_max;
         }
      }
   delete [] marginal;
   marginal= NULL;
   delete [] next_marginal;
   next_marginal= NULL;
}

/*****************************************************************
 *
 *  Computation of the hidden state marginal distributions
 *  for Hidden_markov_out_tree class
 *  using (Default_)observed_trees and the tree index
 *
 **/

double** Hidden_markov_out_tree::state_marginal_distribution(const Trees& trees,
                                                             int index) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;

   int nb_vertices, nb_children, nb_children_max, next_nb_children_max, size;
   register int i, j;
   double **res= NULL;
   double *marginal= new double[nb_state], *next_marginal= new double[nb_state];
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree;
   visitor v;
   vertex_array va;

   // res[j][u] corresponds to the probability that the state variable
   // is equal to j at node u for tree number 'index

   assert(index < trees.get_nb_trees());
   res= new double*[nb_state];
   current_tree= trees.trees[index];
   size= current_tree->get_size();
   for(j= 0; j < nb_state; j++)
      res[j]= new double[size];

   nb_vertices= 0; // number of processed vertices of tree
                   // (also index of current vertex)
   va= v.get_breadthorder(*current_tree);

   for(j= 0; j < nb_state; j++)
   {
      res[j][va[nb_vertices]]= initial[j];
      marginal[j]= initial[j];
   }
   // initialization : at root node, marginal distribution
   // given by "initial" parameter

   nb_children_max= current_tree->get_nb_children(va[nb_vertices]);
   // number of vertices at current depth
   nb_vertices++;
   while (nb_vertices < size)
   {
      nb_children= 0; // number of processed vertices at current depth
      next_nb_children_max= 0;
      // number of vertices at next depth

      for(j= 0; j < nb_state; j++)
      {
         next_marginal[j]= transition[0][j] * marginal[0];
         for(i= 1; i < nb_state; i++)
            next_marginal[j]+= transition[i][j] * marginal[i];
      }
      for(j= 0; j < nb_state; j++)
         marginal[j]= next_marginal[j];

      while (nb_children < nb_children_max)
      {
         next_nb_children_max+= current_tree->get_nb_children(va[nb_vertices]);
         for(j= 0; j < nb_state; j++)
            res[j][va[nb_vertices]]= marginal[j];

         nb_vertices++;
         nb_children++;
      }
      nb_children_max= next_nb_children_max;
   }
   delete [] marginal;
   marginal= NULL;
   delete [] next_marginal;
   next_marginal= NULL;
   return res;
}

/*****************************************************************
 *
 *  Upward step of the upward-downward algorithm
 *  for Hidden_markov_out_tree class
 *  using a Hidden_markov_tree_data object,
 *  the upward probabilities (stored as a byproduct),
 *  the marginal and the output conditional distributions
 *  and the index of considered tree
 *  Return the log-likelihood;
 *  compute the entropy of the state tree and subtree processes.
 *
 **/

double Hidden_markov_out_tree::upward_step(const Hidden_markov_tree_data& trees,
                                           double_array_3d& upward_prob,
                                           double_array_3d& upward_parent_prob,
                                           double_array_3d& state_entropy,
                                           double_array_3d marginal_prob,
                                           double_array_3d output_cond_prob,
                                           double& entropy1,
                                           int index) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   typedef tree_type::children_iterator children_iterator;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;

   int nb_trees= trees._nb_trees, t;
   unsigned int current_size, u;
   register int k, j;
   vid cnode; // current node;
   double_array_2d norm= new double*[nb_trees];
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree;
   children_iterator ch_it, ch_end;
   visitor *v;
   vertex_array va;
   double res= .0, buff;

   // upward_prob[t][j][u] is the probability that the state variable
   // is equal to j at node u for tree t, given the subtree rooted at u

   // upward_parent_prob[t][j][u] is proportional to the probability
   // that the state variable is equal to j at parent node of u for tree t,
   // given the subtree rooted at u

   // state_entropy[t][j][u] is the entropy of the children subtrees state processes
   // given the parent state u and the observed subtree rooted at u

   entropy1= .0;

   assert((index < nb_trees));

   if (upward_prob == NULL)
   {
      upward_prob= new double_array_2d[nb_trees];
      for(t= 0; t < nb_trees; t++)
         upward_prob[t]= NULL;
   }

   if (upward_parent_prob == NULL)
   {
      upward_parent_prob= new double_array_2d[nb_trees];
      for(t= 0; t < nb_trees; t++)
         upward_parent_prob[t]= NULL;
   }

   if (state_entropy == NULL)
   {
      state_entropy= new double_array_2d[nb_trees];
      for(t= 0; t < nb_trees; t++)
         state_entropy[t]= NULL;
   }

   for(t= 0; t < nb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         current_tree= trees.trees[t];
         current_size= current_tree->get_size();
         norm[t]= new double[current_size];

         if (upward_prob[t] == NULL)
         {
            upward_prob[t]= new double*[nb_state];
            for(j= 0; j < nb_state; j++)
               upward_prob[t][j]= NULL;
         }

         if (upward_parent_prob[t] == NULL)
         {
            upward_parent_prob[t]= new double*[nb_state];
            for(j= 0; j < nb_state; j++)
               upward_parent_prob[t][j]= NULL;
         }

         if (state_entropy[t] == NULL)
         {
            state_entropy[t]= new double*[nb_state];
            for(j= 0; j < nb_state; j++)
               state_entropy[t][j]= NULL;
         }

         for(j= 0; j < nb_state; j++)
         {
            if (upward_prob[t][j] == NULL)
               upward_prob[t][j]= new double[current_size];

            if (upward_parent_prob[t][j] == NULL)
               upward_parent_prob[t][j]= new double[current_size];

            if (state_entropy[t][j] == NULL)
               state_entropy[t][j]= new double[current_size];

            upward_parent_prob[t][j][current_tree->root()]= D_DEFAULT;
         }

         v= new generic_visitor<tree_type>;
         traverse_tree(current_tree->root(), *current_tree, *v);
         va= v->get_postorder(*current_tree);
         delete v;

         // a node must be reached before its parent
         assert(va.size() == current_size);

         for(u= 0; u < current_size; u++)
         {
            cnode= va[u];
            norm[t][cnode]= .0;
            if (current_tree->is_leaf(cnode))
            // leaf node
            {
               for(j= 0; j < nb_state; j++)
               {
                  upward_prob[t][j][cnode]=
                     output_cond_prob[t][j][cnode] * marginal_prob[t][j][cnode];
                  norm[t][cnode]+= upward_prob[t][j][cnode];
               }

               for(j= 0; j < nb_state; j++)
                  upward_prob[t][j][cnode] /= norm[t][cnode];

               for(j= 0; j < nb_state; j++)
               {
                  upward_parent_prob[t][j][cnode]= .0;
                  for(k= 0; k < nb_state; k++)
                     if (marginal_prob[t][k][cnode] > 0)
                        upward_parent_prob[t][j][cnode]+= upward_prob[t][k][cnode]
                           * transition[j][k] / marginal_prob[t][k][cnode];
                  state_entropy[t][j][cnode]= .0;
               }
            }
            else // non-terminal node
            {
               for(j= 0; j < nb_state; j++)
               {
                  upward_prob[t][j][cnode]= 1.;
                  state_entropy[t][j][cnode]= .0;
                  tie(ch_it, ch_end)= current_tree->children(cnode);
                  while(ch_it < ch_end)
                  {
                     upward_prob[t][j][cnode]*= upward_parent_prob[t][j][*ch_it];
                     for(k= 0; k < nb_state; k++)
                     {
                        if ((marginal_prob[t][k][*ch_it] > 0)
                           && (upward_parent_prob[t][j][*ch_it] > 0))
                        {
                           // probability of child state given parent state
                           // and subtree rooted at child state
                           buff= upward_prob[t][k][*ch_it] * transition[j][k]
                              / (marginal_prob[t][k][*ch_it] * upward_parent_prob[t][j][*ch_it]);
                           if (buff > 0)
                              state_entropy[t][j][cnode]+= buff
                                 * (state_entropy[t][k][*ch_it] - log(buff));
                        }
                     }
                     ch_it++;
                  }

                  upward_prob[t][j][cnode]*=
                     output_cond_prob[t][j][cnode] * marginal_prob[t][j][cnode];
                  norm[t][cnode]+= upward_prob[t][j][cnode];
               }

               for(j= 0; j < nb_state; j++)
                  upward_prob[t][j][cnode] /= norm[t][cnode];

               if (!current_tree->is_root(cnode))
                  for(j= 0; j < nb_state; j++)
                  {
                     upward_parent_prob[t][j][cnode]= .0;
                     for(k= 0; k < nb_state; k++)
                        if (marginal_prob[t][k][cnode] > 0)
                           upward_parent_prob[t][j][cnode]+= upward_prob[t][k][cnode]
                              * transition[j][k] / marginal_prob[t][k][cnode];
                  }
               else
               {
                  // just checking subroutine coherence
                  for(j= 0; j < nb_state; j++)
                     assert(upward_parent_prob[t][j][cnode] == D_DEFAULT);
                  assert(u == current_size-1);
               }
            } // end non-terminal node
            if (norm[t][cnode] > 0)
               res+= log(norm[t][cnode]);
            else
               res= D_INF;
         } // end for u

         // computation of the conditional hidden state tree entropy
         // root node
         cnode= current_tree->root();
         for (j= 0; j < nb_state; j++)
            if (upward_prob[t][j][cnode] > 0)
               entropy1+= upward_prob[t][j][cnode]
                  * (state_entropy[t][j][cnode] - log(upward_prob[t][j][cnode]));
         delete [] norm[t];
         norm[t]= NULL;
      } // end for t
   delete [] norm;
   norm= NULL;
   return res;
}

/*****************************************************************
 *
 *  Downward step of the upward-downward algorithm
 *  for Hidden_markov_out_tree class
 *  using a Hidden_markov_tree_data object,
 *  the upward probabilities,
 *  the marginal and the output conditional distributions.
 *  The downward probabilities are computed and stored,
 *  as well as the entropy - loglikelihood
 *
 **/

void Hidden_markov_out_tree::downward_step(const Hidden_markov_tree_data& trees,
                                           double_array_3d& downward_prob,
                                           double_array_4d& downward_pair_prob,
                                           double_array_3d upward_prob,
                                           double_array_3d upward_parent_prob,
                                           double_array_3d marginal_prob,
                                           double_array_3d output_cond_prob,
                                           double& entropy2,
                                           int index) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;

   int nb_trees= trees._nb_trees, t;
   unsigned int current_size;
   register int j, i;
   unsigned int u; // k,
   vid cnode, pnode; // current and parent nodes;
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree;
   visitor v;
   vertex_array va;
   // double res= .0;

   // downward_prob[t][j][u] is the probability that the state variable
   // is equal to j at node u for tree t, given the observed tree

   // downward_pair_prob[t][i][j][u] is the probability that the state variable
   // at node of u is equal to j and its parent is equal to i for tree t,
   // given the observed_tree

   entropy2= .0;

   if (downward_prob == NULL)
   {
      downward_prob= new double_array_2d[nb_trees];
      for(t= 0; t < nb_trees; t++)
         downward_prob[t]= NULL;
   }

   if (downward_pair_prob == NULL)
   {
      downward_pair_prob= new double_array_3d[nb_trees];
      for(t= 0; t < nb_trees; t++)
         downward_pair_prob[t]= NULL;
   }

   for(t= 0; t < nb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         current_tree= trees.trees[t];
         current_size= current_tree->get_size();

         if (downward_prob[t] == NULL)
         {
            downward_prob[t]= new double*[nb_state];
            for(j= 0; j < nb_state; j++)
               downward_prob[t][j]= NULL;
         }

         if (downward_pair_prob[t] == NULL)
         {
            downward_pair_prob[t]= new double_array_2d[nb_state];
            for(j= 0; j < nb_state; j++)
               downward_pair_prob[t][j]= NULL;
         }

         for(j= 0; j < nb_state; j++)
         {
            if (downward_prob[t][j] == NULL)
               downward_prob[t][j]= new double[current_size];

            if (downward_pair_prob[t][j] == NULL)
            {
               downward_pair_prob[t][j]= new double*[nb_state];
               for(i= 0; i < nb_state; i++)
                  downward_pair_prob[t][j][i]= NULL;
            }

            for(i= 0; i < nb_state; i++)
               if (downward_pair_prob[t][j][i] == NULL)
                  downward_pair_prob[t][j][i]= new double[current_size];
         }

         va= v.get_breadthorder(*current_tree);

         // a node must be reached after its parent
         assert(va.size() == current_size);

         // initialization

         cnode= current_tree->root();
         for(j= 0; j < nb_state; j++)
         {
            downward_prob[t][j][cnode]= upward_prob[t][j][cnode];
            for(i= 0; i < nb_state; i++)
               downward_pair_prob[t][i][j][cnode]= D_DEFAULT;
            // entropy computation
            if (marginal_prob[t][j][cnode] > 0)
               entropy2-= downward_prob[t][j][cnode]
                  * log(marginal_prob[t][j][cnode]);
            if (output_cond_prob[t][j][cnode] > 0)
               entropy2-= downward_prob[t][j][cnode]
                  * log(output_cond_prob[t][j][cnode]);
         }

         // downward recursion

         for(u= 1; u < current_size; u++)
         {
            cnode= va[u];
            for(j= 0; j < nb_state; j++)
            {
               downward_prob[t][j][cnode]= .0;
               for(i= 0; i < nb_state; i++)
               {
                  downward_pair_prob[t][i][j][cnode]= .0;
                  if ((marginal_prob[t][j][cnode] > 0)
                      && (upward_parent_prob[t][i][cnode] > 0))
                  {
                      pnode= current_tree->parent(cnode);
                      downward_pair_prob[t][i][j][cnode]= (upward_prob[t][j][cnode]
                         * transition[i][j] * downward_prob[t][i][pnode]) /
                         (marginal_prob[t][j][cnode] * upward_parent_prob[t][i][cnode]);
                      downward_prob[t][j][cnode]+= downward_pair_prob[t][i][j][cnode];
                      // entropy computation
                      if (transition[i][j] > 0)
                         entropy2-= downward_pair_prob[t][i][j][cnode]
                            * log(transition[i][j]);
                  }
               }
               // entropy computation
               if (output_cond_prob[t][j][cnode] > 0)
                  entropy2-= downward_prob[t][j][cnode]
                     * log(output_cond_prob[t][j][cnode]);
            }
         } // end for u
      } // end for t
}

/*****************************************************************
 *
 *  Parameter estimation of a hidden Markov out tree by the EM
 *  algorithm from a tree sample, using a Format_error object,
 *  an output stream, the initial hidden Markov out tree model,
 *  a flag on the computation of the counting distributions,
 *  the restoration algorithm (upward-downward or Viterbi)
 *  the iteration number and a flag on the characteristic computation
 *
 **/

Hidden_markov_out_tree*
Hidden_markov_tree_data::hidden_markov_out_tree_estimation(Format_error& error,
                                                           ostream& os,
                                                           const Hidden_markov_out_tree& ihmarkov,
                                                           bool counting_flag,
                                                           int state_trees,
                                                           int nb_iter,
                                                           bool characteristic_flag,
                                                           bool force_param) const
{
   typedef Hidden_markov_tree::double_array_3d double_array_3d;
   typedef Hidden_markov_tree::double_array_4d double_array_4d;
   // typedef Hidden_markov_tree_data::tree_type tree_type;
   // typedef tree_type::vertex_iterator vertex_iterator;

   bool status;
   register int i , j , k , var, t;
   unsigned int u; //m , n,
   int max_nb_value, iter, nb_likelihood_decrease, val; // offset, nb_value
   double likelihood= D_INF, previous_likelihood, observation_likelihood ,
          min_likelihood= 0,  *reestim= NULL, entropy1= D_INF,
          max_marginal_entropy= D_INF;
   double_array_3d state_marginal= NULL, output_cond= NULL,
                   upward_prob= NULL, upward_parent_prob= NULL,
                   downward_prob= NULL, state_entropy= NULL;
   double_array_4d downward_pair_prob= NULL;
   std::deque<int> *path= NULL;
   Chain_reestimation<double> *chain_reestim= NULL;
   Reestimation<double> ***observation_reestim= NULL;
   Histogram *hobservation= NULL;
   Hidden_markov_out_tree *hmarkovt= NULL;
   Hidden_markov_tree_data *otrees= NULL;
   vertex_iterator it, end;
   value v;

 # ifdef DEBUG
   double test[NB_STATE][4];
 # endif

   error.init();

   // test of the number of observed values per variable

   status= false;
   for(var= 0; var < _nb_integral; var++)
      if (get_max_int_value(var)-get_min_int_value(var) > 0)
      {
         status= true;
         break;
      }

   /* for(var= 0; var < _nb_float; var++)
      if (get_max_fl_value(var) - get_min_fl_value(var) > 1)
      {
         status= true;
         break;
      } */
   // is this really necessary ?


   if (!status)
      error.update(STAT_TREES_error[STATR_NB_OUTPUT]);
   else
   {
      if ((ihmarkov._nb_ioutput_process != _nb_integral)
          || (ihmarkov._nb_doutput_process != _nb_float))
          // number of variables for observations and model
          // must match
      {
         status = false;
         error.update(STAT_TREES_error[STATR_NB_OUTPUT_PROCESS]);
      }
      else
      {
         for(var= 0; var < _nb_integral; var++)
         {
            if (((ihmarkov.npprocess[var+1] != NULL) &&
                 (ihmarkov.npprocess[var+1]->get_nb_value() != get_max_int_value(var)+1)) ||
                ((ihmarkov.piprocess[var+1] != NULL) &&
                 (ihmarkov.piprocess[var+1]->nb_value < get_max_int_value(var)+1)))
            {
               status= false;
               ostringstream error_message;
               error_message << STAT_label[STATL_OUTPUT_PROCESS] << " " << var+1 << ": "
                             << STAT_TREES_error[STATR_NB_OUTPUT];
               error.update((error_message.str()).c_str());
            }
            else
               if ((ihmarkov.npprocess[var+1] != NULL) && (characteristics[var] != NULL))
                  for(j= 0; j < get_max_int_value(var); j++)
                     if (characteristics[var]->marginal->frequency[j] == 0)
                     {
                        status= false;
                        ostringstream error_message;
                        error_message << STAT_label[STATL_VARIABLE] << " " << var+1 << ": "
                                      << STAT_error[STATR_MISSING_VALUE] << " " << j;
                        error.update((error_message.str()).c_str());
                     }
         }
      }
   }

   if ((nb_iter != I_DEFAULT) && (nb_iter < 1))
   {
      status = false;
      error.update(STAT_error[STATR_NB_ITERATION]);
   }

   if (status)
   {
      if (_max_size > (unsigned int)COUNTING_MAX_SIZE)
         counting_flag = false;

      // creating the hidden Markov tree

      hmarkovt= new Hidden_markov_out_tree(ihmarkov, false, false);

#   ifdef DEBUG
      cout << *hmarkovt;
#   endif

      // creation des structures de donnees de l'algorithme

      chain_reestim= new Chain_reestimation<double>((hmarkovt->type == 'o' ?  'o' : 'v') ,
                                                    hmarkovt->nb_state, hmarkovt->nb_state);

      observation_reestim= new Reestimation<double>**[hmarkovt->_nb_ioutput_process];
      for(var= 0; var < hmarkovt->_nb_ioutput_process; var++)
      {
         observation_reestim[var]= new Reestimation<double>*[hmarkovt->nb_state];
         for(j= 0; j < hmarkovt->nb_state; j++)
             observation_reestim[var][j]= new Reestimation<double>(get_max_int_value(var)+1);
      }

      max_nb_value= 0;
      for(var= 0; var < hmarkovt->_nb_ioutput_process; var++)
         if ((hmarkovt->piprocess[var+1] != NULL) && (max_nb_value < get_max_int_value(var)+1))
            max_nb_value= get_max_int_value(var) + 1;

      if (max_nb_value > 0)
         hobservation= new Histogram(max_nb_value); // +1, really ?
      else
         hobservation= NULL;

      iter= 0;
      nb_likelihood_decrease= 0;

      do
      {
         iter++;
         previous_likelihood= likelihood;
         likelihood= 0.;

         // initialization of the reestimation quantities

         chain_reestim->init();

         for(var= 0; var < hmarkovt->_nb_ioutput_process; var++)
            for(j= 0; j < hmarkovt->nb_state; j++)
            {
               reestim= observation_reestim[var][j]->frequency;
               for(val= 0; val < get_max_int_value(var)+1; val++)
                  reestim[val]= 0.; // *reestim++ = 0.;
            }

#     ifdef DEBUG
         for(i= 0; i < hmarkovt->nb_state; i++)
            for(j= 0; j < 4; j++)
               test[i][j] = 0.;
#     endif
         hmarkovt->state_marginal_distribution(*this, state_marginal);
         hmarkovt->output_conditional_distribution(*this, output_cond);

         likelihood= hmarkovt->upward_step(*this, upward_prob, upward_parent_prob,
                                           state_entropy, state_marginal,
                                           output_cond, entropy1);
         hmarkovt->downward_step(*this, downward_prob, downward_pair_prob,
                                 upward_prob, upward_parent_prob,
                                 state_marginal, output_cond, entropy1);


         if (likelihood == D_INF)
            break;

         // accumulation of the reestimation quantities for initial distribution
         // and transition probabilities

         for(i= 0; i < hmarkovt->nb_state; i++)
            for(j= 0; j < hmarkovt->nb_state; j++)
               for(t= 0; t < _nb_trees; t++)
               {
                  downward_pair_prob[t][i][j][trees[t]->root()]= 0.;
                  // since this quantity has no meaning, its addition
                  // should not modify chain_reestim->transition[i][j]
                  for(u= 0; u < trees[t]->get_size(); u++)
                      chain_reestim->transition[i][j]
                         += downward_pair_prob[t][i][j][u];
               }

         for(i= 0; i < hmarkovt->nb_state; i++)
            for(t= 0; t < _nb_trees; t++)
            {
               u= trees[t]->root();
               chain_reestim->initial[i]+= downward_prob[t][i][u];
            }

         // accumulation of the reestimation quantities for observation distributions

         for(t= 0; t < _nb_trees; t++)
         {
            tie(it, end)= trees[t]->vertices();
            while (it < end)
            {
               v= trees[t]->get(*it);
               for(var= 0; var < hmarkovt->_nb_ioutput_process; var++)
               {
                  val= v.Int(var);
                  for(k= 0; k < hmarkovt->nb_state; k++)
                     observation_reestim[var][k]->frequency[val]
                        += downward_prob[t][k][*it];
               }
               it++;
            }
         }


         if (likelihood != D_INF)
         {
            if (likelihood < previous_likelihood)
               nb_likelihood_decrease++;
            else
               nb_likelihood_decrease= 0;
         }

         // reestimation of the initial distribution

         reestimation(hmarkovt->nb_state, chain_reestim->initial ,
                      hmarkovt->initial, MIN_PROBABILITY, false);

         // reestimation the transition probabilities

         for(i= 0; i < hmarkovt->nb_state; i++)
            reestimation(hmarkovt->nb_state, chain_reestim->transition[i],
                         hmarkovt->transition[i], MIN_PROBABILITY, false);


         // reestimation of the observation distributions

         for(var = 0; var < hmarkovt->_nb_ioutput_process; var++)
         {
            if (hmarkovt->npprocess[var+1] != NULL)
               for(j= 0; j < hmarkovt->nb_state; j++)
                  reestimation(get_max_int_value(var)+1, observation_reestim[var][j]->frequency,
                               hmarkovt->npprocess[var+1]->observation[j]->mass,
                               MIN_PROBABILITY, false);

            else // (hmarkovt->piprocess[var+1] != NULL)
            {
               for(j= 0; j < hmarkovt->nb_state; j++)
               {
                  observation_reestim[var][j]->nb_value_computation();
                  observation_reestim[var][j]->offset_computation();
                  observation_reestim[var][j]->nb_element_computation();
                  observation_reestim[var][j]->max_computation();
                  observation_reestim[var][j]->mean_computation();
                  observation_reestim[var][j]->variance_computation();

                  hobservation->update(observation_reestim[var][j],
                                       MAX((int)(observation_reestim[var][j]->nb_element
                                                    * MAX(sqrt(observation_reestim[var][j]->variance), 1.) * TREE_OBSERVATION_COEFF),
                                                 TREE_MIN_NB_ELEMENT));
                  if (!force_param)
                     observation_likelihood= hobservation->Reestimation<int>::type_parametric_estimation(hmarkovt->piprocess[var+1]->observation[j],
                                                                                                      0, true, OBSERVATION_THRESHOLD);
                     // above instruction allows the type of the distribution to vary

                  else
                     observation_likelihood= hobservation->Reestimation<int>::parametric_estimation(hmarkovt->piprocess[var+1]->observation[j],
                                                                                                      0, true, OBSERVATION_THRESHOLD);
                     // above instruction prevents the type of the distribution to vary
                     // (not suitable for an automatic initialization of piprocess of UNIFORM type

                  if (observation_likelihood == D_INF)
                     min_likelihood= D_INF;
                  else
                  {
                     hmarkovt->piprocess[var+1]->observation[j]->computation(get_max_int_value(var)+1,
                                                                             OBSERVATION_THRESHOLD);

                     if (hmarkovt->piprocess[var+1]->observation[j]->ident == BINOMIAL)
                        for(k= hmarkovt->piprocess[var+1]->observation[j]->nb_value; k < get_max_int_value(var)+1; k++)
                           hmarkovt->piprocess[var+1]->observation[j]->mass[k]= 0.;
                  }
               }
            }
         }

#     ifdef MESSAGE
         os << STAT_label[STATL_ITERATION] << " " << iter << "   "
            << STAT_TREES_label[STATL_OBSERVED_TREES_LIKELIHOOD] << ": " << likelihood << endl;
#     endif

#     ifdef DEBUG
         if (iter % 5 == 0)
            cout << *hmarkovt;
#     endif

      }
      while ((likelihood != D_INF) && (((nb_iter == I_DEFAULT) && (iter < MARKOV_TREE_NB_ITER) &&
               (((likelihood - previous_likelihood) / -likelihood > MARKOV_TREE_LIKELIHOOD_DIFF) ||
                (min_likelihood == D_INF) || (nb_likelihood_decrease == 1))) ||
              ((nb_iter != I_DEFAULT) && (iter < nb_iter))));

      if (likelihood != D_INF)
      {

#     ifdef MESSAGE
         if (iter > 1)
            os << "\n" << iter << " " << STAT_label[STATL_ITERATIONS] << endl;
         else
            os << "\n" << iter << " " << STAT_label[STATL_ITERATION] << endl;
#     endif

         // reestimation of the initial distribution

         reestimation(hmarkovt->nb_state, chain_reestim->initial ,
                      hmarkovt->initial, MIN_PROBABILITY, true);

         // reestimation the transition probabilities

         for(i= 0; i < hmarkovt->nb_state; i++)
            reestimation(hmarkovt->nb_state, chain_reestim->transition[i],
                         hmarkovt->transition[i], MIN_PROBABILITY, true);

         // reestimation of the observation distributions

         for(var = 0; var < hmarkovt->_nb_ioutput_process; var++)
         {
            if (hmarkovt->npprocess[var+1] != NULL)
               for(j= 0; j < hmarkovt->nb_state; j++)
                  reestimation(get_max_int_value(var)+1, observation_reestim[var][j]->frequency,
                               hmarkovt->npprocess[var+1]->observation[j]->mass,
                               MIN_PROBABILITY, true);

            else // (hmarkovt->piprocess[var+1] != NULL)
               hmarkovt->piprocess[var+1]->nb_value_computation();
         }
      } // end do

      // deallocation of the arrays

      delete chain_reestim;

      for(t= 0; t < _nb_trees; t++)
      {
         for(j= 0; j < hmarkovt->nb_state; j++)
         {
            delete [] state_marginal[t][j];
            state_marginal[t][j]= NULL;
            delete [] output_cond[t][j];
            output_cond[t][j]= NULL;
            delete [] upward_prob[t][j];
            upward_prob[t][j]= NULL;
            delete [] upward_parent_prob[t][j];
            upward_parent_prob[t][j]= NULL;
            delete [] downward_prob[t][j];
            downward_prob[t][j]= NULL;
            for(i= 0; i < hmarkovt->nb_state; i++)
            {
               delete [] downward_pair_prob[t][j][i];
               downward_pair_prob[t][j][i]= NULL;
            }
            delete [] downward_pair_prob[t][j];
            downward_pair_prob[t][j]= NULL;
         }

         delete [] state_marginal[t];
         state_marginal[t]= NULL;
         delete [] output_cond[t];
         output_cond[t]= NULL;
         delete [] upward_prob[t];
         upward_prob[t]= NULL;
         delete [] upward_parent_prob[t];
         upward_parent_prob[t]= NULL;
         delete [] downward_prob[t];
         downward_prob[t]= NULL;
         delete [] downward_pair_prob[t];
         downward_pair_prob[t]= NULL;
      }

      delete [] state_marginal;
      state_marginal= NULL;
      delete [] output_cond;
      output_cond= NULL;
      delete [] upward_prob;
      upward_prob= NULL;
      delete [] upward_parent_prob;
      upward_parent_prob= NULL;
      delete [] downward_prob;
      downward_prob= NULL;
      delete [] downward_pair_prob;
      downward_pair_prob= NULL;

      for(var= 0; var < hmarkovt->_nb_ioutput_process; var++)
      {
         for(j= 0; j < hmarkovt->nb_state; j++)
         {
            delete observation_reestim[var][j];
            observation_reestim[var][j]= NULL;
         }
        delete [] observation_reestim[var];
        observation_reestim[var]= NULL;
      }
      delete [] observation_reestim;
      observation_reestim= NULL;
      // does not seem to work

      delete hobservation;
      hobservation= NULL;

      if (likelihood == D_INF)
      {
         delete hmarkovt;
         hmarkovt= NULL;
         error.update(STAT_error[STATR_ESTIMATION_FAILURE]);
      }
      else
      {
         if ((state_trees == FORWARD_BACKWARD) || (state_trees == VITERBI))
         {
             if (hmarkovt->markov_data != NULL)
                delete hmarkovt->markov_data;

             hmarkovt->markov_data= new Hidden_markov_tree_data(*this, false);
             otrees= hmarkovt->markov_data;

             switch (state_trees)
             {

                case FORWARD_BACKWARD :
                {
                   otrees->build_state_trees();
                   otrees->hidden_likelihood=
                      hmarkovt->upward_downward(*otrees, max_marginal_entropy, entropy1,
                                                likelihood, path, I_DEFAULT, NULL, 'a', 0);
                   if (path != NULL)
                   {
                      delete path;
                      path= NULL;
                   }
                   break;
                };

                case VITERBI :
                {
                   hmarkovt->create_cumul();
                   hmarkovt->log_computation();

                   otrees->build_state_trees();
                   otrees->hidden_likelihood= hmarkovt->viterbi(*otrees);

                   hmarkovt->remove_cumul();
                   break;
                }
             }

             otrees->_nb_states= hmarkovt->nb_state;
             if (otrees->chain_data != NULL)
                delete otrees->chain_data;

             otrees->chain_data= new Chain_data(hmarkovt->type, hmarkovt->nb_state,
                                                hmarkovt->nb_state);
             // otrees->chain_data= new Chain_data(*otrees, 0, 1, hmarkovt);
#            ifdef DEBUG
             otrees->ascii_write(cerr);
#            endif
             otrees->build_observation_histogram();
             // above line generates a segmentation fault
             // otrees->build_state_characteristics();


             // computation of the parametric observation distributions

             for(var= 1; var <= hmarkovt->_nb_ioutput_process; var++)
                if (hmarkovt->piprocess[var] != NULL)
                {
                   for(j= 0; j < hmarkovt->nb_state; j++)
                      hmarkovt->piprocess[var]->observation[j]->computation(otrees->observation[var-1][j]->nb_value,
                                                                            OBSERVATION_THRESHOLD);
                }

#       ifdef MESSAGE
             cout << "\n" << STAT_TREES_label[STATL_STATE_TREES_LIKELIHOOD] << ": " << otrees->hidden_likelihood
                  << " | " << hmarkovt->Hidden_markov_out_tree::state_likelihood_computation(*otrees) << endl;
#       endif

            }

         else // state_trees != FORWARD_BACKWARD and VITERBI
         {
            if (hmarkovt->markov_data != NULL)
               delete hmarkovt->markov_data;

            hmarkovt->markov_data= new Hidden_markov_tree_data(*this);
            otrees= hmarkovt->markov_data;
            // otrees->state_variable_init(INT_VALUE);
         }

         for(var= 1; var <= hmarkovt->_nb_ioutput_process; var++)
            if (hmarkovt->npprocess[var] != NULL)
               for(j= 0; j < hmarkovt->nb_state; j++)
               {
                  hmarkovt->npprocess[var]->observation[j]->cumul_computation();
                  hmarkovt->npprocess[var]->observation[j]->max_computation();
               }

         // computation of the likelihood and of the model characteristics

         otrees->likelihood= hmarkovt->likelihood_computation(*this);

         hmarkovt->component_computation();
         hmarkovt->characteristic_computation(*otrees, counting_flag, I_DEFAULT, false);
      }
   }
   return hmarkovt;
}

/*****************************************************************
 *
 *  Parameter estimation of a hidden Markov out tree by the EM
 *  algorithm from a tree sample, using a Format_error object,
 *  an output stream, the type of markov process ('o'rdinary,
 *  'e'quilibrium), the number of states of the model,
 *  a flag on the structure of the transition probability matrix
 *  and on the computation of the counting distributions,
 *  the restoration algorithm (upward-downward or Viterbi),
 *  the initial self_transition probabilities and the iteration number
 *
 **/

Hidden_markov_out_tree*
Hidden_markov_tree_data::hidden_markov_out_tree_estimation(Format_error& error,
                                                           std::ostream& os,
                                                           char type,
                                                           int nb_state,
                                                           bool left_right,
                                                           bool counting_flag,
                                                           int state_trees,
                                                           double self_transition,
                                                           int nb_iter,
                                                           bool* force_param) const
{
   // note: length of force_param must be checked before call
   bool status= true, fparam= !(force_param==NULL);
   register int var;
   int nb_value[SEQUENCE_NB_VARIABLE];
   Hidden_markov_out_tree *ihmt=NULL, *hmt= NULL;

   error.init();

   if (_nb_float > 0)
   {
      ostringstream correction_message;

      status= false;
      error.update(STAT_TREES_error[STATR_NB_REAL_OUTPUT_PROCESS]);
      correction_message << " HMT with continuous values not implemented";
      error.update((correction_message.str()).c_str());
   }
   if ((nb_state < 2) || (nb_state > NB_STATE))
   {
      status= false;
      error.update(STAT_TREES_error[STATR_NB_STATE]);
   }
   if ((self_transition != D_DEFAULT) &&
        ((self_transition <= 0.) || (self_transition >= 1.)))
   {
      status= false;
      error.update(STAT_TREES_error[STATR_SELF_TRANSITION]);
   }

   if (status)
   {
      for(var= 0; var < _nb_integral; var++)
      {
         nb_value[var]= get_nb_values(var);
         nb_value[var]= characteristics[var]->marginal->nb_value;
      }

      // initial HMT

      ihmt= new Hidden_markov_out_tree(type, nb_state, _nb_integral, _nb_float,
                                       nb_value, force_param);


      if (self_transition == D_DEFAULT)
        self_transition= MAX(1. - 1. / hsize->mean, SELF_TRANSITION);

      ihmt->init(left_right, self_transition);
      // Hidden_markov_tree::init(...)

      // initialization of the observation distributions

      for(var= 0; var < ihmt->_nb_ioutput_process; var++)
      {
         if (ihmt->npprocess[var+1] != NULL)
            ihmt->npprocess[var+1]->init();
         else
            ihmt->piprocess[var+1]->init();
      }
      for(var= 0; var < ihmt->_nb_doutput_process; var++)
         ihmt->pdprocess[var]->init();

      hmt= hidden_markov_out_tree_estimation(error, os, *ihmt, counting_flag,
                                             state_trees , nb_iter, fparam);
      delete ihmt;
      ihmt= NULL;
    }

    return hmt;
}

/*****************************************************************
 *
 *  Hidden state tree restoration by the upward-downward algorithm
 *  for Hidden_markov_out_tree class using a Hidden_markov_tree_data
 *  object, the index of concerned tree (or all trees if I_DEFAULT)
 *  an ouput stream, a file format ('a': ASCII, 's': Spreadsheet, 'g': Gnuplot)
 *  and, for a Gnuplot output, a terminal vertex
 *  Return the completed likelihood, compute the maximal marginal entropy
 *  and the state tree entropy
 *
 **/

double Hidden_markov_out_tree::upward_downward(const Hidden_markov_tree_data& otrees,
                                               double& max_marginal_entropy,
                                               double& entropy1,
                                               double& likelihood,
                                               std::deque<int>*& path,
                                               int index,
                                               std::ostream* os,
                                               char format,
                                               int vertex) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef Hidden_markov_tree_data::vertex_iterator vertex_iterator;
   typedef Hidden_markov_tree_data::state_value state_value;

   register int t, j, cstate= 0;
   double state_likelihood= 0., // completed likelihood
          tree_likelihood, // completed likelihood for each tree
          entropy2= 0., entropy3= 0., // trees entropies
          entropy1t, entropy2t, // entropy for each tree
          pmax;
   state_value val;
   vertex_iterator it, end;

   double *partial_entropy= NULL, *marginal_entropy= NULL;
   double_array_2d conditional_entropy= NULL;
   double_array_3d state_marginal= NULL, output_cond= NULL,
                   upward_prob= NULL, upward_parent_prob= NULL,
                   downward_prob= NULL, state_entropy= NULL;
   // usage: downward_prob[tree][state][vertex], etc.
   double_array_4d downward_pair_prob= NULL;
   // usage: downward_pair_prob[tree][parent_state][child_state][vertex]
   tree_type *current_tree;

   assert(index < otrees.get_nb_trees());
   entropy1= 0.;
   likelihood= 0.;

   for(t= 0; t < otrees.get_nb_trees(); t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         state_marginal_distribution(otrees, state_marginal, t);
         output_conditional_distribution(otrees, output_cond, false, t);

         likelihood= upward_step(otrees, upward_prob, upward_parent_prob,
                                 state_entropy, state_marginal,
                                 output_cond, entropy1t, t);

         downward_step(otrees, downward_prob, downward_pair_prob,
                       upward_prob, upward_parent_prob, state_marginal,
                       output_cond, entropy2t, t);

         // at this stage entropy2 lacks the loglikelihood
         entropy2+= likelihood;

         entropy1+= entropy1t;
         entropy2+= entropy2t;
#        ifdef MESSAGE
         if ((entropy2 < entropy1 - DOUBLE_ERROR)
             || (entropy2 > entropy1 + DOUBLE_ERROR))
             cout << "\nERROR: " << entropy1 << " " << entropy2 << endl;
#        endif
         current_tree= otrees.trees[t];
         // computation of the state subtree entropy (partial entropy)

         entropy3+= partial_entropy_computation(otrees, t, downward_prob,
                                                state_entropy, partial_entropy);

         // computation of the state conditional entropy
         // (given the desencendant state forest)
         conditional_entropy_computation(otrees, state_marginal, upward_prob,
                                         upward_parent_prob, downward_prob,
                                         conditional_entropy, t);

         // hidden state restoration (smoothing)
         tie(it, end)= current_tree->vertices();

         while(it < end)
         {
            pmax= D_INF;
            for(j= 0; j < nb_state; j++)
               if (downward_prob[t][j][*it] > pmax)
               {
                  pmax= downward_prob[t][j][*it];
                  cstate= j;
               }
            val.Int()= cstate;
            (otrees.state_trees[t])->put(*it++, val);
         }

         // completed likelihood of the restored hidden states (by smoothing)
         tree_likelihood= state_likelihood_computation(otrees, t);

         if (tree_likelihood == D_INF)
         {

#           ifdef MESSAGE
            if (os == NULL)
            {
               cout << STAT_label[STATL_STATE] << " " << STAT_TREES_label[STATL_TREE] << " "
                    << t // tree.identifier[i]
                    << ": " << STAT_label[STATL_LIKELIHOOD] << ": " << D_INF << endl;
            }
#           endif
            state_likelihood= D_INF;
         }
         else
            if (state_likelihood!= D_INF)
               state_likelihood+= tree_likelihood;

         marginal_entropy_computation(otrees, t, downward_prob,
                                      marginal_entropy, max_marginal_entropy);

         if (os != NULL)
         {
             switch (format)
             {
                case 'a' :
                {
                   otrees.state_profile_ascii_print(*os, t, nb_state, downward_prob[t]);

                   *os << "\n" << STAT_TREES_label[STATL_STATE_TREE_LIKELIHOOD] << ": " << tree_likelihood;
                   if (tree_likelihood != D_INF)
                      *os << "   (" << STAT_label[STATL_NORMALIZED] << ": " << tree_likelihood / current_tree->get_size() << ")";
                   *os << endl;
                   break;
                }

                case 's' :
                {
                   otrees.state_profile_spreadsheet_print(*os, t, nb_state, downward_prob[t]);

                   *os << "\n" << STAT_TREES_label[STATL_STATE_TREE_LIKELIHOOD] << "\t" << tree_likelihood;
                   if (tree_likelihood != D_INF)
                      *os << "\t" << STAT_label[STATL_NORMALIZED] << "\t" << tree_likelihood / current_tree->get_size();
                   *os << endl;
                   break;
                }

                case 'g' :
                {
                   assert((vertex >=0) && ((unsigned int)vertex < otrees.get_size(index)));
                   if (path != NULL)
                   {
                      delete path;
                      path= new std::deque<int>();
                   }
                   otrees.profile_plot_print(*os, t, nb_state, downward_prob[t],
                                             conditional_entropy[t], marginal_entropy,
                                             partial_entropy,
                                             Hidden_markov_tree_data::key(vertex),
                                             path);
                   break;
                }
             }
         }

/*
         // gini index computation
         tie(it, end)= current_tree->vertices();
         while(it < end)
         {
            for(j= 0; j < nb_state; j++)
               gini_index+= downward_prob[t][j][*it] * (1.-downward_prob[t][j][*it]);
            it++;
         }

         // information computation
         tie(it, end)= current_tree->vertices();
         while(it < end)
         {
            for(j= 0; j < nb_state; j++)
               if (downward_prob[t][j][*it] > 0)
                  information+= downward_prob[t][j][*it] * log(downward_prob[t][j][*it]);
            it++;
         }
*/
        delete [] partial_entropy;
        partial_entropy= NULL;
        delete [] marginal_entropy;
        marginal_entropy= NULL;
      }
   // end for t
#  ifdef MESSAGE
   if ((entropy3 < entropy2 - DOUBLE_ERROR)
       || (entropy3 > entropy2 + DOUBLE_ERROR))
       cout << "\nERROR: " << entropy2 << " " << entropy3 << endl;
#  endif

   for(t= 0; t < otrees.get_nb_trees(); t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         for(j= 0; j < nb_state; j++)
         {
            delete [] state_marginal[t][j];
            state_marginal[t][j]= NULL;
            delete [] output_cond[t][j];
            output_cond[t][j]= NULL;
            delete [] upward_prob[t][j];
            upward_prob[t][j]= NULL;
            delete [] upward_parent_prob[t][j];
            upward_parent_prob[t][j]= NULL;
            delete [] state_entropy[t][j];
            state_entropy[t][j]= NULL;
         }

         delete [] state_marginal[t];
         state_marginal[t]= NULL;
         delete [] output_cond[t];
         output_cond[t]= NULL;
         delete [] upward_prob[t];
         upward_prob[t]= NULL;
         delete [] upward_parent_prob[t];
         upward_parent_prob[t]= NULL;
         delete [] state_entropy[t];
         state_entropy[t]= NULL;
         delete [] conditional_entropy[t];
         conditional_entropy[t]= NULL;
      }

   delete [] state_marginal;
   state_marginal= NULL;
   delete [] output_cond;
   output_cond= NULL;
   delete [] upward_prob;
   upward_prob= NULL;
   delete [] upward_parent_prob;
   upward_parent_prob= NULL;
   delete [] state_entropy;
   state_entropy= NULL;
   delete [] conditional_entropy;
   conditional_entropy= NULL;

/*
   if (path != NULL)
   {
      delete path;
      path= NULL;
   }
*/
#  ifdef DEBUG
   cout << "Computation of the state tree likelihood : "
        << state_likelihood_computation(otrees) << endl;
#  endif

   return state_likelihood;
}

/*****************************************************************
 *
 *  Compute the smoothed probabilities for a Hidden_markov_out_tree
 *  using a Hidden_markov_tree_data object, the state profiles
 *  (smoothed probabilities and entropy profiles),
 *  the index of considered tree and the type of entropy profile
 *  Return the likelihood.
 *
 **/

double Hidden_markov_out_tree::smoothed_probabilities(const Hidden_markov_tree_data& trees,
                                                      double_array_3d& smoothed_prob,
                                                      double_array_2d& marginal_entropy,
                                                      double_array_2d& conditional_entropy,
                                                      double_array_2d& partial_entropy,
                                                      int index,
                                                      int entropy_algo) const

{
   typedef Hidden_markov_tree::double_array_3d double_array_3d;
   typedef Hidden_markov_tree::double_array_4d double_array_4d;

   register int t, i, j;
   const int nb_trees= trees.get_nb_trees();
   double likelihood= D_INF, entropy1, entropy2, entropy3= 0., max_marginal_entropy;
   // double* current_marginal_entropy= NULL; // marginal_entropy for current tree
   double_array_3d state_marginal= NULL, output_cond= NULL,
                   upward_prob= NULL, upward_parent_prob= NULL,
                   state_entropy= NULL;
   double_array_4d downward_pair_prob= NULL;

   assert(index < nb_trees);

   state_marginal_distribution(trees, state_marginal, index);
   output_conditional_distribution(trees, output_cond, false, index);

   // state entropy == entropy of the children state subtrees
   // given parent state u and observed subtree rooted at u
   likelihood= upward_step(trees, upward_prob, upward_parent_prob,
                           state_entropy, state_marginal, output_cond, entropy1,
                           index);
   downward_step(trees, smoothed_prob, downward_pair_prob,
                 upward_prob, upward_parent_prob,
                 state_marginal, output_cond, entropy2,
                 index);
   // conditional entropy == entropy of current vertex state
   // given the children state subtrees and the entire observed tree
   conditional_entropy_computation(trees, state_marginal, upward_prob,
                                   upward_parent_prob, smoothed_prob, conditional_entropy,
                                   index, entropy_algo);

   if (likelihood == D_INF)
   {
      for(t= 0; t < nb_trees; t++)
         if ((index == I_DEFAULT) || (index == t))
         {
            for(j= 0; j < nb_state; j++)
            {
               delete [] smoothed_prob[t][j];
               smoothed_prob[t][j]= NULL;
            }

            delete [] smoothed_prob[t];
            smoothed_prob[t]= NULL;
         }
      delete [] smoothed_prob;
      smoothed_prob= NULL;
   }
   else
   {
      if (marginal_entropy == NULL)
      {
         marginal_entropy= new double*[nb_trees];
         for(t= 0; t < nb_trees; t++)
            marginal_entropy[t]= NULL;
      }
      if (partial_entropy == NULL)
      {
         partial_entropy= new double*[nb_trees];
         for(t= 0; t < nb_trees; t++)
            partial_entropy[t]= NULL;
      }

      for(t= 0; t < nb_trees; t++)
         if ((index == I_DEFAULT) || (index == t))
         {
            // marginal entropy of the smoothed distribution
            marginal_entropy_computation(trees, t, smoothed_prob,
                                         marginal_entropy[t], max_marginal_entropy);
            // partial_entropy[u] == entropy of the state subtree rooted at u
            // given the entire observed subtree
            entropy3+= partial_entropy_computation(trees, t, smoothed_prob,
                                                   state_entropy,
                                                   partial_entropy[t],
                                                   entropy_algo);
            for(j= 0; j < nb_state; j++)
            {
               delete [] state_marginal[t][j];
               state_marginal[t][j]= NULL;
               delete [] output_cond[t][j];
               output_cond[t][j]= NULL;
               delete [] upward_prob[t][j];
               upward_prob[t][j]= NULL;
               delete [] upward_parent_prob[t][j];
               upward_parent_prob[t][j]= NULL;
               delete [] state_entropy[t][j];
               state_entropy[t][j]= NULL;
               for(i= 0; i < nb_state; i++)
               {
                  delete [] downward_pair_prob[t][j][i];
                  downward_pair_prob[t][j][i]= NULL;
               }
               delete [] downward_pair_prob[t][j];
               downward_pair_prob[t][j]= NULL;
            }

            delete [] state_marginal[t];
            state_marginal[t]= NULL;
            delete [] output_cond[t];
            output_cond[t]= NULL;
            delete [] upward_prob[t];
            upward_prob[t]= NULL;
            delete [] upward_parent_prob[t];
            upward_parent_prob[t]= NULL;
            delete [] downward_pair_prob[t];
            downward_pair_prob[t]= NULL;
            delete [] state_entropy[t];
            state_entropy[t]= NULL;
         }
   }

   delete [] state_marginal;
   state_marginal= NULL;
   delete [] output_cond;
   output_cond= NULL;
   delete [] upward_prob;
   upward_prob= NULL;
   delete [] upward_parent_prob;
   upward_parent_prob= NULL;
   delete [] downward_pair_prob;
   downward_pair_prob= NULL;
   delete [] state_entropy;
   state_entropy= NULL;

   return likelihood;
}

/*****************************************************************
 *
 *  Viterbi hidden state restoration algorithm
 *  for Hidden_markov_out_tree class
 *  using a Hidden_markov_tree_data object and the index
 *  of concerned tree (or all trees if I_DEFAULT)
 *
 **/

double Hidden_markov_out_tree::viterbi(const Hidden_markov_tree_data& trees,
                                       int index) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   typedef tree_type::children_iterator children_iterator;
   typedef Hidden_markov_tree_data::state_value value;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;

   int nb_trees= trees._nb_trees, t, current_size;
   register int k, j, u, optimal_root= 0, optimal_parent;
   vid cnode, pnode; // current and parent nodes;
   double_array_3d output_cond= NULL, map= NULL, map2= NULL;
   int ***optimal_states= NULL;
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree= NULL;
   children_iterator ch_it, ch_end;
   visitor *v= NULL;
   value val;
   vertex_array va;
   double likelihood= 0., pmap, *res= NULL;

#  ifdef DEBUG
   tree_type **debug_t= new tree_type*[nb_trees];
   Hidden_markov_tree_data::value debug_val;
   double debug_map;

   debug_val.reset(1, 1);
#  endif

   val.reset(1, 0);

   assert(index < trees.get_nb_trees());

   map= new double_array_2d[nb_trees];
   map2= new double_array_2d[nb_trees];
   optimal_states= new int**[nb_trees];

   res= new double[nb_trees];

   for(t= 0; t < nb_trees; t++)
   {
      map[t]= new double*[nb_state];
      map2[t]= new double*[nb_state];
      optimal_states[t]= new int*[nb_state];
      current_size= (trees.trees[t])->get_size();
      for(j= 0; j < nb_state; j++)
      {
         map[t][j]= new double[current_size];
         map2[t][j]= new double[current_size];
         optimal_states[t][j]= new int[current_size];
      }
   }


   for(t= 0; t < nb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         current_tree= trees.trees[t];
         current_size= current_tree->get_size();

         output_conditional_distribution(trees, output_cond, true, t);
#        ifdef DEBUG
         debug_t[t]= new tree_type(*(current_tree->get_structure()), debug_val);
         cerr << "Current tree: " << endl;
         cerr << *current_tree << endl;
#        endif

         v= new generic_visitor<tree_type>;
         traverse_tree(current_tree->root(), *current_tree, *v);
         va= v->get_postorder(*current_tree);
         // a node must be reached before its parent

         for(u= 0; u < current_size; u++)
         {
            cnode= va[u];
            for(j= 0; j < nb_state; j++)
               map[t][j][cnode]= output_cond[t][j][cnode];

            for(j= 0; j < nb_state; j++)
            {
               if (map[t][j][cnode] != D_INF)
               {
                  tie(ch_it, ch_end)= current_tree->children(cnode);
                  while(ch_it < ch_end)
                  {
                     if (map2[t][j][*ch_it] > D_INF)
                        map[t][j][cnode]+= map2[t][j][*ch_it];
                     else
                        map[t][j][cnode]= D_INF;

                     ch_it++;
                  }
               }
            }

            if (!current_tree->is_root(cnode))
               for(j= 0; j < nb_state; j++)
               {
                  map2[t][j][cnode]= D_INF;
                  for(k= 0; k < nb_state; k++)
                  {
#                    ifdef DEBUG
                     if (((transition[j][k] > 0)
                          && (abs(log(transition[j][k])-cumul_transition[j][k]) > DOUBLE_ERROR))
                        || ((transition[j][k] == 0) && (abs(cumul_transition[j][k]-D_INF) > DOUBLE_ERROR)))
                        cout << "Warning: computation error at transition[" << j << "]["
                             << k << "]." << endl;
#                    endif
                     pmap= map[t][k][cnode] + cumul_transition[j][k];
                     if (pmap > map2[t][j][cnode])
                     {
                        map2[t][j][cnode]= pmap;
                        optimal_states[t][j][cnode]= k;
                     }
                  }
               }
#              ifdef DEBUG
               debug_map= D_INF;
               for(j= 0; j < nb_state; j++)
                  debug_map= max(debug_map, map[t][j][cnode]);
               debug_val.Int(0)= cnode;
               debug_val.Double(0)= debug_map;
               debug_t[t]->put(cnode, debug_val);
#              endif
         } // end for u

         cnode= current_tree->root();
         for(j= 0; j < nb_state; j++)
            map2[t][j][cnode]= D_INF;
         // not to be used in any computation

         // termination : root node
         res[t]= D_INF;
         for(j= 0; j < nb_state; j++)
         {
#           ifdef DEBUG
            if (((initial[j] > 0)
                 && (abs(log(initial[j])-cumul_initial[j]) > DOUBLE_ERROR))
               || ((initial[j] == 0) && (abs(cumul_initial[j]-D_INF) > DOUBLE_ERROR)))
               cout << "Warning: computation error at initial[" << j << "]." << endl;
#           endif
            pmap= map[t][j][cnode] + cumul_initial[j];
            if (pmap > res[t])
            {
               res[t]= pmap;
               optimal_root= j;
            }
         }
         likelihood+= res[t];
#        ifdef DEBUG
         debug_val.Int(0)= cnode;
         debug_val.Double(0)= res[t];
         debug_t[t]->put(cnode, debug_val);
#        endif

         // segmentation
         // each node must be processed before its children
         va= v->get_breadthorder(*current_tree);
         delete v;
         v= NULL;

         for(u= 0; u < current_size; u++)
         {
            cnode= va[u];
            if (current_tree->is_root(cnode))
               val.Int()= optimal_root;
            else
            {
               pnode= current_tree->parent(va[u]);
               optimal_parent= (trees.state_trees[t]->get(pnode)).Int();
               val.Int()= optimal_states[t][optimal_parent][cnode];
            }
            trees.state_trees[t]->put(va[u], val);
         }

         for(j= 0; j < nb_state; j++)
         {
            delete [] map[t][j];
            delete [] map2[t][j];
            delete [] optimal_states[t][j];
         }
         delete [] map[t];
         delete [] map2[t];
         delete [] optimal_states[t];
      }
   // end for t
   delete [] res;
   delete [] map;
   delete [] map2;
   delete [] optimal_states;
#  ifdef DEBUG
   delete [] debug_t;
   cout << "Computation of the state tree likelihood : "
        << state_likelihood_computation(trees) << endl;
#  endif

   return likelihood;
}

/*****************************************************************
 *
 *  Compute the number of possible state trees
 *  for Hidden_markov_out_trees using a given Hidden_markov_tree_data
 *
 **/

long double Hidden_markov_out_tree::nb_state_trees(const Hidden_markov_tree_data& trees,
                                                   int index) const
{

   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;
   typedef tree_type::children_iterator children_iterator;

   register int t, j, k; //i
   unsigned int current_size= 0;
   unsigned int u; // k,
   int nb_trees= trees._nb_trees;
   long double nb_state_trees= 0., tmp_upward;
   vid cnode; //, pnode; // current and parent nodes;
   children_iterator ch_it, ch_end;
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree;
   visitor *v;
   vertex_array va, depth;
   long double **upward_prob= NULL;
   double_array_3d output_cond_prob= NULL;

   assert(index < trees.get_nb_trees());

   for(t= 0; t < nb_trees; t++)
   {
      if ((index == t) || (index == I_DEFAULT))
      {
         current_tree= trees.trees[t];
         current_size= current_tree->get_size();
         output_conditional_distribution(trees, output_cond_prob, true, t);

         upward_prob= new long double*[nb_state];
         for(j= 0; j < nb_state; j++)
            upward_prob[j]= new long double[current_size];

         v= new generic_visitor<tree_type>;
         traverse_tree(current_tree->root(), *current_tree, *v);
         va= v->get_postorder(*current_tree);
         delete v;
         v= NULL;

         // a node must be reached before its parent
         assert(va.size() == current_size);

         // downward recursion

         for(u= 0; u < current_size; u++)
         {
            cnode= va[u];
#           ifdef DEBUG
            cout << "Handling vertex " << cnode << ": ";
#           endif
            for(j= 0; j < nb_state; j++)
            {
               if (output_cond_prob[t][j][cnode] == D_INF)
                  // number of state trees with root in state j
                  // rooted at vertex cnode
                  upward_prob[j][cnode]= 0;
               else
               {
                  // number of state trees with root in state j
                  // rooted at vertex cnode
                  upward_prob[j][cnode]= 1;
                  tie(ch_it, ch_end)= current_tree->children(cnode);
                  while(ch_it < ch_end)
                  {
                     // number of state trees with root in any state
                     // rooted at given child
                     tmp_upward= 0;
                     for(k= 0; k < nb_state; k++)
                        if (cumul_transition[j][k] > D_INF)
                           tmp_upward+= upward_prob[k][*ch_it];
                     upward_prob[j][cnode]*= tmp_upward;
                     ch_it++;
                  }
               }
#              ifdef DEBUG
               cout << upward_prob[j][cnode] << "\t";
#              endif
            }
#           ifdef DEBUG
            cout << endl;
#           endif
         } // end for u
         cnode= current_tree->root();
         for(j= 0; j < nb_state; j++)
            if ((output_cond_prob[t][j][cnode] > D_INF)
                && (cumul_initial[j] > D_INF))
               nb_state_trees+= upward_prob[j][cnode];
         for(j= 0; j < nb_state; j++)
         {
            delete [] upward_prob[j];
            upward_prob[j]= NULL;
         }
         delete [] upward_prob;
         upward_prob= NULL;
      }
   } // end for t
   return nb_state_trees;
}

/*****************************************************************
 *
 *  Viterbi upward-downward algorithm for Hidden_markov_out_tree,
 *  using given observations, the likelihood, the index of considered tree,
 *  an ouput stream, a file format ('a': ASCII, 's': Spreadsheet, 'g': Gnuplot)
 *  and, for a Gnuplot output, a terminal vertex.
 *  Return the optimal state trees and the completed likelihood ratios
 *  (log (P_Viterbi_upward_downward / likelihood))
 *  as tree variables
 *
 **/

Hidden_markov_tree_data*
Hidden_markov_out_tree::viterbi_upward_downward(const Hidden_markov_tree_data& trees,
                                                std::vector<ostringstream*>& messages,
                                                double likelihood,
                                                double& state_tree_likelihood,
                                                std::deque<int>*& path,
                                                int index,
                                                std::ostream* os,
                                                char format,
                                                int vertex) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   typedef tree_type::vertex_iterator vertex_iterator;
   typedef tree_type::children_iterator children_iterator;
   typedef Hidden_markov_tree_data::state_value value;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;

   register int k, j, u, optimal_root= 0; //, optimal_parent;
   register int offset, var, st;
   const int added_int_variables= 1;
   const int added_float_variables= nb_state,
             added_variables= added_int_variables+added_float_variables;
   int nb_trees= trees._nb_trees, t, tid, current_size, inb_trees;
   vid cnode, pnode; // current and parent nodes;
   double pmap;
   vertex_iterator it, end;
   children_iterator ch_it, ch_end;
   value val;
   vertex_array va;
   visitor *v= NULL;
   double *res= NULL;
   int *itype= NULL;
   double_array_3d output_cond= NULL, map_upward= NULL, map2_upward= NULL,
                   map_downward= NULL, ratio= NULL;
   int **optimal_states= NULL; // 2d array
   ostringstream *m= NULL;
   Int_fl_container i(trees._nb_integral+added_int_variables,
                      trees._nb_float+added_float_variables),
                    s;
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree= NULL;
   Default_tree **otrees= NULL;
   Hidden_markov_tree_data *res_trees= NULL;
   // const Hidden_markov_tree *hmarkovt= NULL;
   Unlabelled_typed_edge_tree *tmp_utree= NULL;
   int inb_variables;

   val.reset(1, 0);

   assert(index < trees.get_nb_trees());

   state_tree_likelihood= 0.;
   map_downward= new double_array_2d[nb_trees];
   map_upward= new double_array_2d[nb_trees];
   map2_upward= new double_array_2d[nb_trees];
   ratio= new double_array_2d[nb_trees];
   optimal_states= new int*[nb_trees];

   // usage: ratio[tree][state][vertex]

   res= new double[nb_trees];
   // res[tree] == maximal completed likelihood

   if (index == I_DEFAULT)
      inb_trees= nb_trees;
   else
      inb_trees= 1;

   inb_variables= trees._nb_integral + trees._nb_float + added_variables;
   otrees= new Default_tree*[inb_trees];
   itype= new int[inb_variables];

   for(var=0; var < added_int_variables; var++)
      itype[var]= STATE;

   offset= added_int_variables;
   for(var=0; var < trees._nb_integral; var++)
      itype[var+offset]= trees._type[var];

   offset+= trees._nb_integral;
   for(var=0; var < added_float_variables; var++)
      itype[var+offset]= REAL_VALUE;

   offset= added_float_variables;
   for(var=0; var < trees._nb_float; var++)
      itype[var+offset]= trees._type[var+trees._nb_integral];

   for(t= 0; t < nb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         map_upward[t]= new double*[nb_state];
         map_downward[t]= new double*[nb_state];
         map2_upward[t]= new double*[nb_state];
         ratio[t]= new double*[nb_state];
         current_size= (trees.trees[t])->get_size();
         optimal_states[t]= new int[current_size];
         for(j= 0; j < nb_state; j++)
         {
            map_upward[t][j]= new double[current_size];
            map_downward[t][j]= new double[current_size];
            map2_upward[t][j]= new double[current_size];
            ratio[t][j]= new double[current_size];
         }
      }

   for(t= 0; t < nb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         output_conditional_distribution(trees, output_cond, true, t);

#        ifdef DEBUG
         Hidden_markov_tree_data ctrees= Hidden_markov_tree_data(trees);
         Typed_edge_one_int_tree *index_tree= NULL;
         index_tree= ctrees.get_identifier_tree(t);
         cout << "Handle tree " << t << ": \n";
         index_tree->display(cout, index_tree->root());
         delete index_tree;
#        endif
         current_tree= trees.trees[t];
         current_size= current_tree->get_size();

         v= new generic_visitor<tree_type>;
         traverse_tree(current_tree->root(), *current_tree, *v);
         va= v->get_postorder(*current_tree);
         delete v;
         v= NULL;
         // a node must be reached before its parent

         for(u= 0; u < current_size; u++)
         {
            cnode= va[u];
            for(j= 0; j < nb_state; j++)
               map_upward[t][j][cnode]= output_cond[t][j][cnode];

            for(j= 0; j < nb_state; j++)
            {
               if (map_upward[t][j][cnode] != D_INF)
               {
                  tie(ch_it, ch_end)= current_tree->children(cnode);
                  while(ch_it < ch_end)
                  {
                     if (map2_upward[t][j][*ch_it] > D_INF)
                        map_upward[t][j][cnode]+= map2_upward[t][j][*ch_it];
                     else
                        map_upward[t][j][cnode]= D_INF;

                     ch_it++;
                  }
               }
            }

            if (!current_tree->is_root(cnode))
               for(j= 0; j < nb_state; j++)
               {
                  map2_upward[t][j][cnode]= D_INF;
                  for(k= 0; k < nb_state; k++)
                  {
                     pmap= map_upward[t][k][cnode] + cumul_transition[j][k];
                     if (pmap > map2_upward[t][j][cnode])
                        map2_upward[t][j][cnode]= pmap;
                  }
               }
#           ifdef DEBUG
            cout << "vertex " << cnode << ": ";
            cout << "delta = " << "[";
            for(j= 0; j < nb_state-1; j++)
               cout << map_upward[t][j][cnode] << "\t";
            cout << map_upward[t][j][cnode] << "]";
            cout << "; delta2 = " << "[";
            for(j= 0; j < nb_state-1; j++)
               cout << map2_upward[t][j][cnode] << "\t";
            cout << map2_upward[t][j][cnode] << "]\n";
#           endif
         } // end for u

         cnode= current_tree->root();
         for(j= 0; j < nb_state; j++)
            map2_upward[t][j][cnode]= D_INF;
         // not to be used in any computation

         // termination : root node
         res[t]= D_INF;
         for(j= 0; j < nb_state; j++)
         {
            pmap= map_upward[t][j][cnode] + cumul_initial[j];
            if (pmap > res[t])
            {
               res[t]= pmap;
               optimal_root= j;
            }
         }
         state_tree_likelihood+= res[t];
#        ifdef DEBUG
         cout << "\n state_tree_likelihood = " << res[t] << endl;
#        endif

         if (state_tree_likelihood > D_INF)
         {

            // downward step
            // a node must be reached after its parent
            v= new generic_visitor<tree_type>;
            traverse_tree(current_tree->root(), *current_tree, *v);
            va= v->get_breadthorder(*current_tree);
            delete v;
            v= NULL;

            // initialization

            cnode= current_tree->root();
            for(j= 0; j < nb_state; j++)
               map_downward[t][j][cnode]= cumul_initial[j];

            // downward recursion

            for(u= 1; u < current_size; u++)
            {
               cnode= va[u];
               for(j= 0; j < nb_state; j++)
                  map_downward[t][j][cnode]= D_INF;

               for(j= 0; j < nb_state; j++)
               {
                  for(k= 0; k < nb_state; k++)
                  {
                     pnode= current_tree->parent(cnode);
                     pmap= cumul_transition[k][j] + output_cond[t][k][pnode]
                           + map_downward[t][k][pnode];

                     tie(ch_it, ch_end)= current_tree->children(pnode);
                     // handle each brother of current vertex
                     while(ch_it < ch_end)
                     {
                        if (*ch_it != cnode)
                        {
                           if (map2_upward[t][k][*ch_it] > D_INF)
                              pmap+= map2_upward[t][k][*ch_it];
                           else
                              pmap= D_INF;
                        }
                        ch_it++;
                     }
                     if (pmap > map_downward[t][j][cnode])
                        map_downward[t][j][cnode]= pmap;
                  }
               }
#              ifdef DEBUG
               cout << "vertex " << cnode << ": ";
               cout << "gamma = " << "[";
               for(j= 0; j < nb_state-1; j++)
                  cout << map_downward[t][j][cnode] << "\t";
               cout << map_downward[t][j][cnode] << "]";
               cout << "; log(max prob) = " << "[";
               for(j= 0; j < nb_state-1; j++)
                  cout << map_downward[t][j][cnode] + map_upward[t][j][cnode] << "\t";
               cout << map_downward[t][j][cnode] + map_upward[t][j][cnode] << "]\n";
#              endif
            } // end for u


            // computation of the completed likelihood ratios
            // and optimal states

            for(u= 0; u < current_size; u++)
            {
               cnode= va[u];
               optimal_states[t][cnode]= 0;
               pmap= D_INF;
               for(j= 0; j < nb_state; j++)
               {
                  ratio[t][j][cnode]= map_upward[t][j][cnode] + map_downward[t][j][cnode];
                  //  map_upward est bloque a D_INF !
                  if (pmap < ratio[t][j][cnode])
                  {
                     optimal_states[t][cnode]= j;
                     pmap= ratio[t][j][cnode];
                  }
                  ratio[t][j][cnode]= exp(ratio[t][j][cnode]-likelihood);
               }
               val.Int()= optimal_states[t][cnode];
               trees.state_trees[t]->put(va[u], val);
            }
         } // end downward step

         m= new ostringstream;
         switch (format)
         {
         case 'a':
            {
               *m << "\n"
                  << STAT_TREES_label[STATL_MAX_POSTERIOR_STATE_PROBABILITY] << "\n\n";

               messages.push_back(m);
               m= new ostringstream;
               *m << "\n" << STAT_TREES_label[STATL_STATE_TREE_LIKELIHOOD] << ": " << res[t]
                  << "   (" << exp(res[t] - likelihood) << ")" << endl;
               messages.push_back(m);
               break;
            }
         case 's':
            {
               *m << "\n"
                  << STAT_TREES_label[STATL_MAX_POSTERIOR_STATE_PROBABILITY] << "\n\n";

               messages.push_back(m);
               m= new ostringstream;
               *m << "\n" << STAT_TREES_label[STATL_STATE_TREE_LIKELIHOOD] << "\t" << res[t]
                  << "\t" << exp(res[t] - likelihood) << "\t" << endl;
               messages.push_back(m);
               break;
            }
         }

         // build the trees with the results of the algorithm as variables

         tmp_utree= trees.trees[t]->get_structure();
         if (index == I_DEFAULT)
            tid= t;
         else // (t == index)
            tid= 0;
         otrees[tid]= new Default_tree(trees._nb_integral+added_int_variables,
                                       trees._nb_float+added_float_variables,
                                       trees.trees[t]->root(), 1);
         otrees[tid]->set_structure(*tmp_utree, i);
         tie(it, end)= trees.trees[t]->vertices();
         while (it < end)
         {
            s= trees.trees[t]->get(*it);
            // add integer variables
            i.Int(0)= (trees.state_trees[t]->get(*it)).Int();
            // copy existing integer variables
            for(var= 0; var < trees._nb_integral; var++)
               i.Int(var+added_int_variables)=s.Int(var);
            // add floating variables
            for(st= 0; st < added_float_variables; st++)
            if (ratio[t][st][*it] > 0)
               i.Double(st)= log(ratio[t][st][*it]);
            else
               i.Double(st)= D_INF;
            // copy existing floating variables
            for(var= 0; var < trees._nb_float; var++)
               i.Double(var+added_float_variables)=s.Double(var);
            otrees[tid]->put(*it++, i);
         }

         delete tmp_utree;
         tmp_utree= NULL;

         for(j= 0; j < nb_state; j++)
         {
            delete [] map_upward[t][j];
            delete [] map_downward[t][j];
            delete [] map2_upward[t][j];
         }
         delete [] map_upward[t];
         delete [] map_downward[t];
         delete [] map2_upward[t];
         delete [] optimal_states[t];
      }
   // end for t

   res_trees= new Hidden_markov_tree_data(inb_trees, itype, otrees);
   res_trees->markov= new Hidden_markov_out_tree(*this, false, false);

   res_trees->state_trees= new Typed_edge_one_int_tree*[inb_trees];
   if (index == I_DEFAULT)
   {
      for(t= 0; t < inb_trees; t++)
         res_trees->state_trees[t]= new Typed_edge_one_int_tree(*(trees.state_trees[t]));
   }
   else // (t == index)
      res_trees->state_trees[0]= new Typed_edge_one_int_tree(*(trees.state_trees[index]));
   res_trees->likelihood= likelihood;
   res_trees->hidden_likelihood= state_tree_likelihood;
   res_trees->_nb_states= nb_state;

   // res->chain_data= new Chain_data(*res, 0, 1, markov);
   res_trees->chain_data= new Chain_data(type, nb_state, nb_state);
   res_trees->build_characteristics();
   res_trees->build_size_histogram();
   res_trees->build_nb_children_histogram();
   res_trees->build_observation_histogram();

   res_trees->markov->characteristic_computation(*res_trees, true);

   if ((format == 'g') && (index != I_DEFAULT))
      res_trees->profile_plot_print(*os, 0, nb_state, ratio[index], vertex, path);

   // delete path;
   // path= NULL;

   delete [] itype;
   itype= NULL;

   if (index == I_DEFAULT)
      for(t= 0; t < inb_trees; t++)
      {
         {
            delete otrees[t];
            otrees[t]= NULL;
         }
      }
   else // (t == index)
   {
      delete otrees[0];
      otrees[0]= NULL;
   }

   for(t= 0; t < inb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         for(j= 0; j < nb_state; j++)
         {
            delete ratio[t][j];
            ratio[t][j]= NULL;
         }
         delete ratio[t];
         ratio[t]= NULL;
      }

   delete [] otrees;
   otrees= NULL;

   delete [] res;
   delete [] map_upward;
   delete [] map_downward;
   delete [] map2_upward;
   delete [] ratio;
   delete [] optimal_states;

   return res_trees;
}

/*****************************************************************
 *
 *  Generalized Viterbi algorithm for Hidden_markov_out_trees.
 *  Compute the nb_state_trees best trees associated with
 *  the given trees, using the likelihood and the tree index
 *
 **/

Hidden_markov_tree_data*
Hidden_markov_out_tree::generalized_viterbi(const Hidden_markov_tree_data& trees,
                                            std::vector<ostringstream*>& messages,
                                            int nb_state_trees,
                                            double likelihood,
                                            int index) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   typedef tree_type::vertex_iterator vertex_iterator;
   typedef tree_type::children_iterator children_iterator;
   typedef Hidden_markov_tree_data::state_value value;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;
   // typedef int**** int_array_4d;
   // typedef int*** int_array_3d;
   typedef std::vector<int> int_array;
   typedef std::vector<std::vector<std::vector<int> > > int_array_3d;
   typedef std::vector<std::vector<std::vector<std::vector<int> > > >
           int_array_4d;

   register int k, j, u, m;
   register int var;
   const int _nb_integral= nb_state_trees;
   unsigned int max_digits= 0; // format the display
   int nb_trees= trees._nb_trees, t, tid, e, current_size, inb_trees,
       effective_nb_state_trees, state, pstate, crank, // current_rank,
       nb_cell, nb_children, ch_id, ci,
       combination, cp_combination, max_combination, next_max_combination;
   vid cnode, pnode; // current and parent vertices;
   double state_tree_likelihood= 0., pmap, mapm, mpmap,
          hidden_likelihood, likelihood_cumul;
   bool skip_combination;
   // bool *used_combination= NULL;
   // bool **active_cell= NULL;
   int *itype= NULL; //, *nb_cell_array= NULL,
   //    *max_children_ranks= NULL, *cnt_ch= NULL;
   // double *likelihood_array= NULL, *ratio_array= NULL, *cumul_array= NULL;
   // long double *max_nb_state_trees= NULL;
   double_array_3d map_upward= NULL, map2_upward= NULL;
                   // previous_upward= NULL;
   double_array_3d output_cond= NULL;
   // int **val_states= NULL;
   int_array_3d optimal_states, optimal_ranks;
   int_array_4d children_ranks;
   ostringstream *msg= NULL;
   vertex_iterator it, end;
   children_iterator ch_it, ch_end;
   vertex_array va;
   Int_fl_container i(_nb_integral, 0);
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree= NULL;
   visitor *v= NULL;
   std::vector<bool> used_combination;
   std::vector<int> inv_children_ids, rank, brank, nb_cell_array,
                    max_children_ranks, cnt_ch;
   std::vector<double> likelihood_array, ratio_array, cumul_array;
   std::vector<long double> max_nb_state_trees;
   std::vector<std::vector<bool> > active_cell;
   std::vector<std::vector<int> > val_states, children_ids;
   Default_tree **otrees= NULL;
   Hidden_markov_tree_data *res_trees= NULL;
   Unlabelled_typed_edge_tree *tmp_utree= NULL;

   assert(index < trees.get_nb_trees());

   if (index == I_DEFAULT)
      inb_trees= nb_trees;
   else
      inb_trees= 1;

   otrees= new Default_tree*[inb_trees];
   itype= new int[_nb_integral];

   for(var=0; var < _nb_integral; var++)
      itype[var]= STATE;

   likelihood_array.resize(nb_state_trees);
   ratio_array.resize(nb_state_trees);
   cumul_array.resize(nb_state_trees);
   nb_cell_array.resize(nb_state_trees);

   for(t= 0; t < nb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         output_conditional_distribution(trees, output_cond, true, t);

         msg= new ostringstream;
         current_size= (trees.trees[t])->get_size();
         max_nb_state_trees.resize(current_size);
         // in fact the maximal number of each state subtree

         map_upward= new double_array_2d[nb_state];
         map2_upward= new double_array_2d[nb_state];
         for(j= 0; j < nb_state; j++)
         {
            map_upward[j]= new double*[current_size];
            map2_upward[j]= new double*[current_size];
         }
         rank.resize(current_size);
         inv_children_ids.resize(current_size);
         children_ids.resize(current_size);
         optimal_states.resize(current_size);
         val_states.resize(current_size);
         optimal_ranks.resize(current_size);
         children_ranks.resize(current_size);
         brank.resize(current_size);

         for(u= 0; u < current_size; u++)
         {
            optimal_states[u].resize(nb_state);
            for(j= 0; j < nb_state; j++)
            {
               optimal_states[u][j].resize(nb_state_trees);
               map_upward[j][u]= new double[nb_state_trees];
               map2_upward[j][u]= new double[nb_state_trees];
            }
         }

         optimal_ranks.resize(current_size);
         for(u= 0; u < current_size; u++)
         {
            optimal_ranks[u].resize(nb_state);
            children_ranks[u].resize(nb_state);
            for(j= 0; j < nb_state; j++)
            {
               optimal_ranks[u][j].resize(nb_state_trees);
               children_ranks[u][j].resize(nb_state_trees);
            }
         }

         for(u= 0; u < current_size; u++)
            val_states[u].resize(nb_state_trees);

         active_cell.resize(current_size);
         for(u= 0; u < current_size; u++)
         {
           active_cell[u].resize(nb_state);
           for(j= 0; j < nb_state; j++)
              active_cell[u][j]= false;
         }
#        ifdef DEBUG
         Hidden_markov_tree_data ctrees= Hidden_markov_tree_data(trees);
         Typed_edge_one_int_tree *index_tree= NULL;
         index_tree= ctrees.get_identifier_tree(t);
         cout << "Handle tree " << t << ": \n";
         index_tree->display(cout, index_tree->root());
         delete index_tree;
#        endif
         current_tree= trees.trees[t];
         current_size= current_tree->get_size();

         v= new generic_visitor<tree_type>;
         traverse_tree(current_tree->root(), *current_tree, *v);
         va= v->get_postorder(*current_tree);
         delete v;
         v= NULL;
         // a node must be reached before its parent

         effective_nb_state_trees= 1;

         for(u= 0; u < current_size; u++)
         {

            cnode= va[u];
            nb_children= current_tree->get_nb_children(cnode);

            max_nb_state_trees[cnode]= nb_state;
            tie(ch_it, ch_end)= current_tree->children(cnode);
               while(ch_it < ch_end)
                  max_nb_state_trees[cnode]*= max_nb_state_trees[*ch_it++];
            if (nb_state_trees > max_nb_state_trees[cnode])
               effective_nb_state_trees= (int)max_nb_state_trees[cnode];
            else
               effective_nb_state_trees= nb_state_trees;
            // base for coding children combinations
            e= effective_nb_state_trees; // +1;

            for(j= 0; j < nb_state; j++)
               for(m= 0; m < nb_state_trees; m++)
               {
                  if (nb_children > 0)
                     children_ranks[cnode][j][m].resize(nb_children);
                  // else
                  //   children_ranks[cnode][j][m].resize(0);
               }

            if (nb_children > 0)
            {
               children_ids[cnode].resize(nb_children);
               cnt_ch.resize(nb_children);
               // sum_ranks= 0;
               max_children_ranks.resize(nb_children);
               // table of already used (optimal) combination of children orders
               used_combination.resize((int)pow((double)e, (double)nb_children));
            }

#           ifdef DEBUG
            cout << "vertex " << cnode << ": \n";
            cout << "Output probabilities: \n";
#           endif
            for(j= 0; j < nb_state; j++)
            {
               map_upward[j][cnode][0]= output_cond[t][j][cnode];
#              ifdef DEBUG
               cout << output_cond[t][j][cnode] << "\t";
#              endif
               for(m= 1; m < nb_state_trees; m++)
               {
                  map_upward[j][cnode][m]= D_INF;
                  map2_upward[j][cnode][m]= D_INF;
               }
            }
#           ifdef DEBUG
            cout << endl;
#           endif

            if (nb_children > 0)
            {
               tie(ch_it, ch_end)= current_tree->children(cnode);
               // correspondence between children iterator and
               // vertex ids
               ch_id= 0;
               while(ch_it < ch_end)
               {
                  children_ids[cnode][ch_id]= *ch_it;
                  inv_children_ids[*ch_it++]= ch_id++;
               }
            }

            for(j= 0; j < nb_state; j++)
            {
               if (nb_children > 0)
               {
                  max_combination= (int)pow((double)e, (double)nb_children);
                  for(combination= 0; combination < max_combination; combination++)
                     used_combination[combination]= false;
               }
               for(ch_id= 0; ch_id < nb_children; ch_id++)
                  max_children_ranks[ch_id]= 1;
               next_max_combination= 1;
               for(m= 0; m < effective_nb_state_trees; m++)
               {
                  if (output_cond[t][j][cnode] > D_INF)
                     if (nb_children > 0)
                     {
                        mpmap= D_INF;
                        map_upward[j][cnode][m]= output_cond[t][j][cnode];
                        // code referring to a combination of children orders
                        max_combination= next_max_combination; // (int)pow((double)m+1, (double)nb_children);
                        for(combination= 0; combination < max_combination; combination++)
                           if (!used_combination[combination])
                           {
                              pmap= 0.;
                              cp_combination= combination;
                              skip_combination= false;
                              for(ch_id= nb_children-1; ch_id >= 0; ch_id--)
                              {
                                 cnt_ch[ch_id]= cp_combination / (int)pow((double)e, (double)ch_id);
                                 cp_combination-= cnt_ch[ch_id] * (int)pow((double)e, (double)ch_id);
                                 if (cnt_ch[ch_id] >= max_children_ranks[ch_id])
                                    skip_combination= true;
                              }
                              if (!skip_combination)
                              {
#                                ifdef DEBUG
                                 cout << "Exploring children rank combination: ";
#                                endif
                                 for(ch_id= nb_children-1; ch_id >=0 ; ch_id--)
                                 {
                                    ci= children_ids[cnode][ch_id];
                                    if (map2_upward[j][ci][cnt_ch[ch_id]] > D_INF)
                                       pmap+= map2_upward[j][ci][cnt_ch[ch_id]];
                                    else
                                       pmap= D_INF;
#                                   ifdef DEBUG
                                    cout << cnt_ch[ch_id] << "\t";
#                                   endif
                                 }
                                 if (pmap >= mpmap)
                                 {
                                    mpmap= pmap;
                                    for(ch_id= nb_children-1; ch_id >=0; ch_id--)
                                       children_ranks[cnode][j][m][ch_id]= cnt_ch[ch_id];
                                 }
#                                ifdef DEBUG
                                 cout << "\n";
#                                endif
                              }
                           }
                        map_upward[j][cnode][m]+= mpmap;
                        next_max_combination= 0;
                        combination= 0;
                        for(ch_id= nb_children-1; ch_id >=0 ; ch_id--)
                        {
                           max_children_ranks[ch_id]=
                              max(children_ranks[cnode][j][m][ch_id]+2,
                                  max_children_ranks[ch_id]);
                           next_max_combination+=
                              (int)max_children_ranks[ch_id] * (int)pow((double)e, double(ch_id));
                           combination+= children_ranks[cnode][j][m][ch_id]
                                         * (int)pow((double)e, (double)ch_id);
                        }
                        used_combination[combination]= true;
                     } // end (nb_chidren > 0)
               } // end for m
            } // end for j

            if (!current_tree->is_root(cnode))
               for(j= 0; j < nb_state; j++)
               {
                  for(k= 0; k < nb_state; k++)
                     rank[k]= 0;
                  for(m= 0; m < effective_nb_state_trees; m++)
                  {
                     map2_upward[j][cnode][m]= D_INF;
                     for(k= 0; k < nb_state; k++)
                     {
                        pmap= map_upward[k][cnode][rank[k]]
                              + cumul_transition[j][k];
                        if (pmap > map2_upward[j][cnode][m])
                        {
                           map2_upward[j][cnode][m]= pmap;
                           optimal_states[cnode][j][m]= k;
                           optimal_ranks[cnode][j][m]= rank[k];
                        }
                     }
                     if (map2_upward[j][cnode][m] > D_INF)
                        rank[optimal_states[cnode][j][m]]++;
                  }
               }
#           ifdef DEBUG
            cout << "delta = " << endl;
            for(j= 0; j < nb_state; j++)
            {
               cout << "state " << j << ": [";
               for(m= 0; m < nb_state_trees-1; m++)
                  cout << map_upward[j][cnode][m] << "\t";
               cout << map_upward[j][cnode][m] << "]" << endl;
            }
            cout << endl << "delta2 = " << endl;
            for(j= 0; j < nb_state; j++)
            {
               cout << "state " << j << ": [";
               for(m= 0; m < nb_state_trees-1; m++)
                  cout << map2_upward[j][cnode][m] << "\t";
               cout << map2_upward[j][cnode][m] << "]" << endl;
            }
            cout << endl;
#           endif
         } // end for u

         cnode= current_tree->root();
         for(j= 0; j < nb_state; j++)
            for(m= 0; m < nb_state_trees-1; m++)
               map2_upward[j][cnode][m]= D_INF;
         // not to be used in any computation

         for(k= 0; k < nb_state; k++)
            rank[k]= 0;

         likelihood_cumul= 0.;

         for(m= 0; m < effective_nb_state_trees; m++)
         {
            // termination: root node
            cnode= current_tree->root();
            mapm= D_INF;
            for(j= 0; j < nb_state; j++)
            {
               if (map_upward[j][cnode][rank[j]] > D_INF)
                  pmap= map_upward[j][cnode][rank[j]] + cumul_initial[j];
               else
                  pmap= D_INF;
               if (pmap > mapm)
               {
                  mapm= pmap;
                  state= j;
               }
            }
#           ifdef DEBUG
            cout << "\n hidden_likelihood = " << mapm << endl;
#           endif
            if (m == 0)
            {
               hidden_likelihood= mapm;
               if (mapm <= D_INF)
                  break;
               else
                  state_tree_likelihood+= mapm;
            }

            // restoration
            v= new generic_visitor<tree_type>;
            traverse_tree(current_tree->root(), *current_tree, *v);
            va= v->get_breadthorder(*current_tree);
            delete v;
            v= NULL;

            cnode= va[0];
            active_cell[cnode][state]= true;
            brank[cnode]= rank[state];
            rank[state]++;
            val_states[cnode][m]= state;

            for(u= 1; u < current_size; u++)
            {
               cnode= va[u];
               pnode= current_tree->parent(cnode);
               pstate= val_states[pnode][m];
               ch_id= inv_children_ids[cnode];
               crank= children_ranks[pnode][pstate][brank[pnode]][ch_id];
               if (nb_children > 0)
                  brank[cnode]= optimal_ranks[cnode][pstate][crank];
               state= optimal_states[cnode][pstate][crank];
               val_states[cnode][m]= state;
               active_cell[cnode][state]= true;
               // brank= current_rank;
            }
            nb_cell= 0;
            for(u= 0; u < current_size; u++)
               for(j= 0; j < nb_state; j++)
                  if (active_cell[u][j])
                     nb_cell++;
            likelihood_cumul+= exp(mapm);

            likelihood_array[m]= mapm;
            ratio_array[m]= exp(mapm-hidden_likelihood);
            cumul_array[m]= exp(log(likelihood_cumul) - likelihood);
            nb_cell_array[m]= nb_cell;
         } // end for m

         // computation of the maximal width for table printing
         for(m= 0; m < effective_nb_state_trees-1; m++)
         {
            max_digits= max((unsigned int)(log((double)abs(likelihood_array[m]))/log(10.)),
                            max_digits);
            max_digits= max((unsigned int)(log((double)nb_cell_array[m])/log(10.)),
                            max_digits);
         }
         max_digits+=5; // decimal part, sign and dot
#        ifdef DEBUG
         cout << "maximal number of digits: " << max_digits << endl;
#        endif
         *msg << " ";
         for(m= 0; m < effective_nb_state_trees-1; m++)
            *msg << setw(max_digits+2) << m;
         *msg << setw(max_digits+2) << m << endl;
         *msg << "|";
         for(m= 0; m < effective_nb_state_trees-1; m++)
            *msg << fixed << setw(max_digits+2) << setprecision(2) << likelihood_array[m]; // << "\t";
         *msg << fixed << setw(max_digits+2) << setprecision(2) << likelihood_array[m] << "|" << endl;
         *msg << "|";
         for(m= 0; m < effective_nb_state_trees-1; m++)
            *msg << fixed << setw(max_digits+2) << setprecision(2) << ratio_array[m]; // << "\t";
         *msg << fixed << setw(max_digits+2) << setprecision(2) << ratio_array[m] << "|" << endl;
         *msg << "|";
         for(m= 0; m < effective_nb_state_trees-1; m++)
            *msg << setw(max_digits+2) << cumul_array[m]; // << "\t";
         *msg << setw(max_digits+2) << cumul_array[m] << "|" << endl;
         *msg << "|";
         for(m= 0; m < effective_nb_state_trees-1; m++)
            *msg << setw(max_digits+2) << nb_cell_array[m]; // << "\t";
         *msg << setw(max_digits+2) << nb_cell_array[m] << "|" << endl;

         messages.push_back(msg);
         // build the trees with the results of the algorithm as variables

         tmp_utree= trees.trees[t]->get_structure();
         if (index == I_DEFAULT)
            tid= t;
         else // (index == t)
            tid= 0;

         otrees[tid]= new Default_tree(_nb_integral, 0,
                                       trees.trees[t]->root(), 1);
         otrees[tid]->set_structure(*tmp_utree, i);
         tie(it, end)= trees.trees[t]->vertices();
         while (it < end)
         {
            for(var= 0; var < _nb_integral; var++)
               i.Int(var)= val_states[*it][var];
            otrees[tid]->put(*it++, i);
         }

         delete tmp_utree;
         tmp_utree= NULL;

         for(j= 0; j < nb_state; j++)
         {
            for(u= 0; u < current_size; u++)
            {
               delete [] map_upward[j][u];
               delete [] map2_upward[j][u];
            }
            delete [] map_upward[j];
            delete [] map2_upward[j];
         }
         delete [] map_upward;
         delete [] map2_upward;

      }
   // end for t
   res_trees= new Hidden_markov_tree_data(inb_trees, itype, otrees);
   res_trees->markov= new Hidden_markov_out_tree(*this, false, false);

   res_trees->state_trees= new Typed_edge_one_int_tree*[inb_trees];
   if (index == I_DEFAULT)
   {
      for(t= 0; t < inb_trees; t++)
         res_trees->state_trees[t]= new Typed_edge_one_int_tree(*(trees.state_trees[t]));
   }
   else // (t == index)
      res_trees->state_trees[0]= new Typed_edge_one_int_tree(*(trees.state_trees[index]));
   res_trees->likelihood= likelihood;
   res_trees->hidden_likelihood= state_tree_likelihood;
   res_trees->_nb_states= nb_state;

   // res->chain_data= new Chain_data(*res, 0, 1, markov);

   res_trees->chain_data= new Chain_data(type, nb_state, nb_state);
   res_trees->build_characteristics();
   res_trees->build_size_histogram();
   res_trees->build_nb_children_histogram();
   res_trees->build_observation_histogram();

   // res_trees is not the realization of a Markov model
   // res_trees->markov->characteristic_computation(*res_trees, true);

   delete [] itype;
   itype= NULL;
   if (index == I_DEFAULT)
      for(t= 0; t < inb_trees; t++)
      {
         {
            delete otrees[t];
            otrees[t]= NULL;
         }
      }
   else // (t == index)
   {
      delete otrees[0];
      otrees[0]= NULL;
   }

   delete [] otrees;
   otrees= NULL;


   return res_trees;
}

/*****************************************************************
 *
 *  Compute the joint probability of the hidden state
 *  and observed trees for Hidden_markov_out_tree class
 *  using a Hidden_markov_tree_data object
 *  (and the state_tree field as given state process)
 *
 **/

double Hidden_markov_out_tree::state_likelihood_computation(const Hidden_markov_tree_data& otrees) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef Hidden_markov_tree_data::state_tree_type state_tree_type;
   typedef Hidden_markov_tree_data::state_value value;
   typedef Hidden_markov_tree_data::vertex_iterator vertex_iterator;

   register int t, j, state, parent_state;
   double likelihood= 0;
   vertex_iterator it, end;
   tree_type *current_tree;
   state_tree_type *current_state_tree;
   double_array_3d output_cond= NULL;
   Hidden_markov_out_tree *hmarkov= new Hidden_markov_out_tree(*this);

   hmarkov->create_cumul();
   hmarkov->log_computation();

   hmarkov->output_conditional_distribution(otrees, output_cond, true);

   for(t= 0; t < otrees.get_nb_trees(); t++)
   {
      current_tree= otrees.trees[t];
      current_state_tree= otrees.state_trees[t];
      tie(it, end)= current_tree->vertices();
      while (it < end)
      {
          state= (current_state_tree->get(*it)).Int();
          likelihood+= output_cond[t][state][*it];
          if (current_tree->is_root(*it))
          {
             if (initial[state] > 0)
                likelihood+= log(initial[state]);
             else
             {
                likelihood= D_INF;
                break;
             }
          }
          else
          {
             parent_state= (current_state_tree->get(current_tree->parent(*it))).Int();
             if (transition[parent_state][state] > 0)
                likelihood+= log(transition[parent_state][state]);
             else
             {
                likelihood= D_INF;
                break;
             }
          }
          it++;
      }
   }

   for(t= 0; t < otrees.get_nb_trees(); t++)
   {
      for(j= 0; j < nb_state; j++)
      {
         delete [] output_cond[t][j];
         output_cond[t][j]= NULL;
      }

      delete [] output_cond[t];
      output_cond[t]= NULL;
   }

   delete [] output_cond;
   output_cond= NULL;

   hmarkov->remove_cumul();
   delete hmarkov;
   hmarkov= NULL;

   return likelihood;
}

/*****************************************************************
 *
 *  Compute the marginal entropy for Hidden_markov_out_tree class
 *  using the observed trees and downward probabilities.
 *
 **/
void Hidden_markov_out_tree::marginal_entropy_computation(const Hidden_markov_tree_data& otrees,
                                                          int t,
                                                          double_array_3d downward_prob,
                                                          double*& marginal_entropy,
                                                          double& max_marginal_entropy) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_iterator vertex_iterator;

   register int j;
   vertex_iterator it, end;
   tree_type *current_tree;
   // usage: marginal_entropy[vertex]

   max_marginal_entropy= 0.;
   current_tree= otrees.trees[t];
   if (marginal_entropy == NULL)
      marginal_entropy= new double[current_tree->get_size()];
   tie(it, end)= current_tree->vertices();
   while(it < end)
   {
      marginal_entropy[*it]= .0;
      for(j= 0; j < nb_state; j++)
         if (downward_prob[t][j][*it] > 0)
            marginal_entropy[*it]-= downward_prob[t][j][*it]
               * log(downward_prob[t][j][*it]);
      if (marginal_entropy[*it] > max_marginal_entropy)
         max_marginal_entropy= marginal_entropy[*it];
      it++;
   }
}

/*****************************************************************
 *
 *  Compute the entropy of partial state processes for Hidden_markov_out_tree class
 *  using the observed trees and downward probabilities, by an upward algorithm
 *
 **/

double Hidden_markov_out_tree::upward_partial_entropy_computation(const Hidden_markov_tree_data& otrees,
                                                                  int t,
                                                                  double_array_3d downward_prob,
                                                                  double_array_3d state_entropy,
                                                                  double*& partial_entropy) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_iterator vertex_iterator;

   register int j;
   double entropy3;
   vertex_iterator it, end;
   tree_type *current_tree;
   // usage: partial_entropy[u] == entropy of the state subtree rooted at u
   // given the entire observed subtree

   current_tree= otrees.trees[t];
   if (partial_entropy == NULL)
      partial_entropy= new double[current_tree->get_size()];

   tie(it, end)= current_tree->vertices();
   while(it < end)
   {
      partial_entropy[*it]= .0;
      for(j= 0; j < nb_state; j++)
         if (downward_prob[t][j][*it] > 0)
            partial_entropy[*it]+= downward_prob[t][j][*it]
               * (state_entropy[t][j][*it] - log(downward_prob[t][j][*it]));
      it++;
   }
   entropy3= partial_entropy[current_tree->root()];

   return entropy3;
}

/*****************************************************************
 *
 *  Compute the entropy of partial state processes for Hidden_markov_out_tree class
 *  using the observed trees and downward probabilities, by a downward algorithm
 *
 **/

double Hidden_markov_out_tree::downward_partial_entropy_computation(const Hidden_markov_tree_data& otrees,
                                                                    int t,
                                                                    double_array_3d downward_prob,
                                                                    double_array_3d state_entropy,
                                                                    double*& partial_entropy) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_iterator vertex_iterator;

   register int j;
   double entropy3;
   vertex_iterator it, end;
   tree_type *current_tree;
   // usage: partial_entropy[u] == entropy of the state subtree rooted at u
   // given the entire observed subtree

   current_tree= otrees.trees[t];
   if (partial_entropy == NULL)
      partial_entropy= new double[current_tree->get_size()];

   tie(it, end)= current_tree->vertices();
   while(it < end)
   {
      partial_entropy[*it]= .0;
      for(j= 0; j < nb_state; j++)
         if (downward_prob[t][j][*it] > 0)
            partial_entropy[*it]+= downward_prob[t][j][*it]
               * (state_entropy[t][j][*it] - log(downward_prob[t][j][*it]));
      it++;
   }
   entropy3= partial_entropy[current_tree->root()];

   return entropy3;
}

/*****************************************************************
 *
 *  Compute the conditional entropy for Hidden_markov_out_tree class
 *  using the observed trees, the marginal, upward and downward probabilities
 *  and the index of considered tree, using an upward algorithm
 *
 **/

void Hidden_markov_out_tree::upward_conditional_entropy_computation(const Hidden_markov_tree_data& trees,
                                                                    double_array_3d marginal_prob,
                                                                    double_array_3d upward_prob,
                                                                    double_array_3d upward_parent_prob,
                                                                    double_array_3d downward_prob,
                                                                    double_array_2d& conditional_entropy,
                                                                    int index) const

{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   // typedef tree_type::vertex_iterator vertex_iterator;
   typedef tree_type::children_iterator children_iterator;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;

   register int j= 0, s;
   unsigned int current_size;
   unsigned int u; // k,
   int nb_trees= trees._nb_trees, t, ch_id, // relative children identifier
       nb_children, combination, cp_combination, // a coded combination of children states
       max_state_combination;
   vid cnode; // current node;
   double joint_childrenp, base_jointp;
   children_iterator ch_it, ch_end;
   int *state_ch= NULL, *inv_children_ids= NULL, *children_ids= NULL;
   double *const jointp= new double[nb_state];
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree;
   visitor *v;
   vertex_array va;
   // usage: conditional_entropy[tree][vertex]
   // == entropy of current vertex state given the children state subtrees
   // and the entire observed tree

   if (conditional_entropy == NULL)
   {
      conditional_entropy= new double*[nb_trees];
      for(t= 0; t < nb_trees; t++)
         conditional_entropy[t]= NULL;
   }

   for(t= 0; t < nb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         current_tree= trees.trees[t];
         current_size= current_tree->get_size();

         if (conditional_entropy[t] == NULL)
            conditional_entropy[t]= new double[current_size];

         v= new generic_visitor<tree_type>;
         traverse_tree(current_tree->root(), *current_tree, *v);
         va= v->get_postorder(*current_tree);
         delete v;

         // a node must be reached before its parent
         assert(va.size() == current_size);

         for(u= 0; u < current_size; u++)
         {
            cnode= va[u];
            conditional_entropy[t][cnode]= 0.;
            nb_children= current_tree->get_nb_children(cnode);
#           ifdef DEBUG
            double sump= 0.;
#           endif
            if (nb_children > 0)
            {
               children_ids= new int[nb_children];
               inv_children_ids= new int[current_size];
               state_ch= new int[nb_children];

               // children state combinations are coded in base "nb_state"
               max_state_combination= (int)pow((double)nb_state,
                                               (double)nb_children);

               tie(ch_it, ch_end)= current_tree->children(cnode);
               // correspondence between children iterator and
               // vertex ids
               ch_id= 0;
               while(ch_it < ch_end)
               {
                  children_ids[ch_id]= *ch_it; // relative to absolute children id
                  inv_children_ids[*ch_it++]= ch_id++; // absolute to relative children id
                  // ch_id++;
                  // ch_it++;
               }

               for(combination= 0; combination < max_state_combination; combination++)
               {
                  joint_childrenp= 0.;
                  cp_combination= combination;
                  for(ch_id= nb_children-1; ch_id >= 0; ch_id--)
                  {
                     // compute the children state combination associated with current code
                     // with use of relative indexation of children
                     state_ch[ch_id]= cp_combination / (int)pow((double)nb_state,
                                                                (double)ch_id);
                     cp_combination-= state_ch[ch_id] * (int)pow((double)nb_state,
                                                                 (double)ch_id);
                  }

#                 ifdef DEBUG
                  cout << "Exploring children state combination: ";
                  for(ch_id= nb_children-1; ch_id >=0 ; ch_id--)
                     cout << state_ch[ch_id] << "\t";
                  cout << "\n";
#                 endif
                  // factor in the joint probability of the parent and children states
                  // that does not depend on the current state
                  base_jointp= 1.;
                  tie(ch_it, ch_end)= current_tree->children(cnode);
                  while(ch_it < ch_end)
                  {
                     if (base_jointp > 0)
                     {
                        ch_id= inv_children_ids[*ch_it]; // relative child identifier
                        s= state_ch[ch_id];
                        if (marginal_prob[t][s][*ch_it] > 0)
                           base_jointp*= upward_prob[t][s][*ch_it] / marginal_prob[t][s][*ch_it];
                        else
                           base_jointp= 0.;
                     }
                     ch_it++;
                  }

                  for(j= 0; j < nb_state; j++)
                  {
                     // joint probability of the parent and children states
                     jointp[j]= base_jointp * downward_prob[t][j][cnode];
                     tie(ch_it, ch_end)= current_tree->children(cnode);
                     while(ch_it < ch_end)
                     {
                        if (jointp[j] > 0)
                        {
                           ch_id= inv_children_ids[*ch_it]; // absolute child identifier
                           s= state_ch[ch_id];
                           if (upward_parent_prob[t][s][*ch_it] > 0)
                              jointp[j]*= transition[j][s]
                                          / upward_parent_prob[t][j][*ch_it];
                           else
                              jointp[j]= 0.;
                        }
                        ch_it++;
                     }
                     // joint probability of the children states
                     joint_childrenp+= jointp[j];
                  }
                  for(j= 0; j < nb_state; j++)
                     if (jointp[j] > 0)
                        conditional_entropy[t][cnode]-=
                           jointp[j] * (log(jointp[j]) - log(joint_childrenp));
#                 ifdef DEBUG
                  sump+= joint_childrenp;
#                 endif
               }// end for current combination
#              ifdef DEBUG
               if ((abs(sump - 1.) > DOUBLE_ERROR) && (nb_children > 0))
                  cout << "ERROR: joint parent and children state probabilities "
                       << "does not sum to one: " << sump << endl;
#              endif

            }
            else // leaf node
               for(j= 0; j < nb_state; j++)
                  if (downward_prob[t][j][cnode] > 0)
                     conditional_entropy[t][cnode]-=
                         downward_prob[t][j][cnode] * log(downward_prob[t][j][cnode]);
            if (nb_children > 0)
            {
               delete [] state_ch;
               state_ch= NULL;
               delete [] children_ids;
               children_ids= NULL;
               delete [] inv_children_ids;
               inv_children_ids= NULL;
            }
         } // end for u
      } // end for t
   delete [] jointp;
}

/*****************************************************************
 *
 *  Compute the conditional entropy for Hidden_markov_out_tree class
 *  using the observed trees, the marginal, upward and downward probabilities
 *  and the index of considered tree, using a downward algorithm
 *
 **/

void Hidden_markov_out_tree::downward_conditional_entropy_computation(const Hidden_markov_tree_data& trees,
                                                                      double_array_3d marginal_prob,
                                                                      double_array_3d upward_prob,
                                                                      double_array_3d upward_parent_prob,
                                                                      double_array_3d downward_prob,
                                                                      double_array_2d& conditional_entropy,
                                                                      int index) const

{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef tree_type::vertex_descriptor vid;
   // typedef tree_type::vertex_iterator vertex_iterator;
   typedef tree_type::children_iterator children_iterator;
   typedef generic_visitor<tree_type> visitor;
   typedef visitor::vertex_array vertex_array;

   register int j= 0, s;
   unsigned int current_size;
   unsigned int u; // k,
   int nb_trees= trees._nb_trees, t, ch_id, // relative children identifier
       nb_children, combination, cp_combination, // a coded combination of children states
       max_state_combination;
   vid cnode; // current node;
   double joint_childrenp, base_jointp;
   children_iterator ch_it, ch_end;
   int *state_ch= NULL, *inv_children_ids= NULL, *children_ids= NULL;
   double *const jointp= new double[nb_state];
   Typed_edge_int_fl_tree<Int_fl_container> *current_tree;
   visitor *v;
   vertex_array va;
   // usage: conditional_entropy[tree][vertex]
   // == entropy of current vertex state given the children state subtrees
   // and the entire observed tree

   if (conditional_entropy == NULL)
   {
      conditional_entropy= new double*[nb_trees];
      for(t= 0; t < nb_trees; t++)
         conditional_entropy[t]= NULL;
   }

   for(t= 0; t < nb_trees; t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         current_tree= trees.trees[t];
         current_size= current_tree->get_size();

         if (conditional_entropy[t] == NULL)
            conditional_entropy[t]= new double[current_size];

         v= new generic_visitor<tree_type>;
         traverse_tree(current_tree->root(), *current_tree, *v);
         va= v->get_postorder(*current_tree);
         delete v;

         // a node must be reached before its parent
         assert(va.size() == current_size);

         for(u= 0; u < current_size; u++)
         {
            cnode= va[u];
            conditional_entropy[t][cnode]= 0.;
            nb_children= current_tree->get_nb_children(cnode);
#           ifdef DEBUG
            double sump= 0.;
#           endif
            if (nb_children > 0)
            {
               children_ids= new int[nb_children];
               inv_children_ids= new int[current_size];
               state_ch= new int[nb_children];

               // children state combinations are coded in base "nb_state"
               max_state_combination= (int)pow((double)nb_state,
                                               (double)nb_children);

               tie(ch_it, ch_end)= current_tree->children(cnode);
               // correspondence between children iterator and
               // vertex ids
               ch_id= 0;
               while(ch_it < ch_end)
               {
                  children_ids[ch_id]= *ch_it; // relative to absolute children id
                  inv_children_ids[*ch_it++]= ch_id++; // absolute to relative children id
                  // ch_id++;
                  // ch_it++;
               }

               for(combination= 0; combination < max_state_combination; combination++)
               {
                  joint_childrenp= 0.;
                  cp_combination= combination;
                  for(ch_id= nb_children-1; ch_id >= 0; ch_id--)
                  {
                     // compute the children state combination associated with current code
                     // with use of relative indexation of children
                     state_ch[ch_id]= cp_combination / (int)pow((double)nb_state,
                                                                (double)ch_id);
                     cp_combination-= state_ch[ch_id] * (int)pow((double)nb_state,
                                                                 (double)ch_id);
                  }

#                 ifdef DEBUG
                  cout << "Exploring children state combination: ";
                  for(ch_id= nb_children-1; ch_id >=0 ; ch_id--)
                     cout << state_ch[ch_id] << "\t";
                  cout << "\n";
#                 endif
                  // factor in the joint probability of the parent and children states
                  // that does not depend on the current state
                  base_jointp= 1.;
                  tie(ch_it, ch_end)= current_tree->children(cnode);
                  while(ch_it < ch_end)
                  {
                     if (base_jointp > 0)
                     {
                        ch_id= inv_children_ids[*ch_it]; // relative child identifier
                        s= state_ch[ch_id];
                        if (marginal_prob[t][s][*ch_it] > 0)
                           base_jointp*= upward_prob[t][s][*ch_it] / marginal_prob[t][s][*ch_it];
                        else
                           base_jointp= 0.;
                     }
                     ch_it++;
                  }

                  for(j= 0; j < nb_state; j++)
                  {
                     // joint probability of the parent and children states
                     jointp[j]= base_jointp * downward_prob[t][j][cnode];
                     tie(ch_it, ch_end)= current_tree->children(cnode);
                     while(ch_it < ch_end)
                     {
                        if (jointp[j] > 0)
                        {
                           ch_id= inv_children_ids[*ch_it]; // absolute child identifier
                           s= state_ch[ch_id];
                           if (upward_parent_prob[t][s][*ch_it] > 0)
                              jointp[j]*= transition[j][s]
                                          / upward_parent_prob[t][j][*ch_it];
                           else
                              jointp[j]= 0.;
                        }
                        ch_it++;
                     }
                     // joint probability of the children states
                     joint_childrenp+= jointp[j];
                  }
                  for(j= 0; j < nb_state; j++)
                     if (jointp[j] > 0)
                        conditional_entropy[t][cnode]-=
                           jointp[j] * (log(jointp[j]) - log(joint_childrenp));
#                 ifdef DEBUG
                  sump+= joint_childrenp;
#                 endif
               }// end for current combination
#              ifdef DEBUG
               if ((abs(sump - 1.) > DOUBLE_ERROR) && (nb_children > 0))
                  cout << "ERROR: joint parent and children state probabilities "
                       << "does not sum to one: " << sump << endl;
#              endif

            }
            else // leaf node
               for(j= 0; j < nb_state; j++)
                  if (downward_prob[t][j][cnode] > 0)
                     conditional_entropy[t][cnode]-=
                         downward_prob[t][j][cnode] * log(downward_prob[t][j][cnode]);
            if (nb_children > 0)
            {
               delete [] state_ch;
               state_ch= NULL;
               delete [] children_ids;
               children_ids= NULL;
               delete [] inv_children_ids;
               inv_children_ids= NULL;
            }
         } // end for u
      } // end for t
   delete [] jointp;
}

double Hidden_markov_out_tree::state_likelihood_computation(const Hidden_markov_tree_data& otrees,
                                                            int index) const
{
   typedef Hidden_markov_tree_data::tree_type tree_type;
   typedef Hidden_markov_tree_data::state_tree_type state_tree_type;
   typedef Hidden_markov_tree_data::state_value value;
   typedef Hidden_markov_tree_data::vertex_iterator vertex_iterator;
   // Typed_edge_int_fl_tree<Int_fl_container> *current_tree;
   tree_type *current_tree;
   state_tree_type *current_state_tree;
   vertex_iterator it, end;
   register int t, j, state, parent_state;
   double likelihood= 0;
   double_array_3d output_cond= NULL;

   assert(index < otrees.get_nb_trees());

   for(t= 0; t < otrees.get_nb_trees(); t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         output_conditional_distribution(otrees, output_cond, true, t);

         current_tree= otrees.trees[t];
         current_state_tree= otrees.state_trees[t];
         tie(it, end)= current_tree->vertices();
         while (it < end)
         {
             state= (current_state_tree->get(*it)).Int();
             likelihood+= output_cond[t][state][*it];
             if (current_tree->is_root(*it))
             {
                if (initial[state] > 0)
                   likelihood+= log(initial[state]);
                else
                {
                   likelihood= D_INF;
                   break;
                }
             }
             else
             {
                parent_state= (current_state_tree->get(current_tree->parent(*it))).Int();
                if (transition[parent_state][state] > 0)
                   likelihood+= log(transition[parent_state][state]);
                else
                {
                   likelihood= D_INF;
                   break;
                }
             }
             it++;
         }
   }

   for(t= 0; t < otrees.get_nb_trees(); t++)
      if ((index == I_DEFAULT) || (index == t))
      {
         for(j= 0; j < nb_state; j++)
         {
            delete [] output_cond[t][j];
            output_cond[t][j]= NULL;
         }

         delete [] output_cond[t];
         output_cond[t]= NULL;
      }

   delete [] output_cond;
   output_cond= NULL;

   return likelihood;
}
