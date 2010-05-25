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



#ifndef SEQUENCE_LABEL_H
#define SEQUENCE_LABEL_H



/****************************************************************
 *
 *  Identificateurs des mots cles (format des fichiers) :
 */


enum {
  SEQW_MARKOV_CHAIN ,
  SEQW_EQUILIBRIUM_MARKOV_CHAIN ,
  SEQW_HIDDEN_MARKOV_CHAIN ,
  SEQW_EQUILIBRIUM_HIDDEN_MARKOV_CHAIN ,

  SEQW_SEMI_MARKOV_CHAIN ,
  SEQW_EQUILIBRIUM_SEMI_MARKOV_CHAIN ,
  SEQW_HIDDEN_SEMI_MARKOV_CHAIN ,
  SEQW_EQUILIBRIUM_HIDDEN_SEMI_MARKOV_CHAIN ,

  SEQW_NONHOMOGENEOUS_MARKOV_CHAIN ,
  SEQW_HOMOGENEOUS ,
  SEQW_NONHOMOGENEOUS ,

  SEQW_OCCUPANCY_DISTRIBUTION ,

  SEQW_INDEX_PARAMETER ,

  SEQW_TOP_PARAMETERS ,
  SEQW_AXILLARY_PROBABILITY ,
  SEQW_RHYTHM_RATIO
};


extern const char *SEQ_word[];
extern const char *SEQ_index_parameter_word[];



/****************************************************************
 *
 *  Identificateurs des labels :
 */


enum {
  SEQL_STATE_SEQUENCE_LIKELIHOOD ,
  SEQL_STATE_SEQUENCES_LIKELIHOOD ,
  SEQL_OBSERVED_SEQUENCES_LIKELIHOOD ,
  SEQL_IID_INFORMATION ,

  SEQL_SMOOTHED ,
  SEQL_OBSERVED ,
  SEQL_THEORETICAL ,
  SEQL_SMOOTHED_OBSERVED_PROBABILITIES ,

  SEQL_ORDINARY_RENEWAL ,
  SEQL_EQUILIBRIUM_RENEWAL ,
  SEQL_OBSERVATION_TIME ,
  SEQL_INTER_EVENT ,
  SEQL_BACKWARD ,
  SEQL_FORWARD ,
  SEQL_RECURRENCE_TIME ,
  SEQL_LENGTH_BIASED ,
  SEQL_OBSERVATION_INTER_EVENT ,
  SEQL_2_CENSORED_INTER_EVENT ,
  SEQL_1_CENSORED_INTER_EVENT ,
  SEQL_COMPLETE_INTER_EVENT ,
  SEQL_TIME_UP ,
  SEQL_NB_EVENT ,
  SEQL_DURING ,
  SEQL_TIME_UNIT ,
  SEQL_NB_EVENT_MIXTURE ,
  SEQL_NO_EVENT_PROBABILITY ,
  SEQL_EVENT_PROBABILITY ,

  SEQL_MARKOV_CHAIN ,
  SEQL_HIDDEN_MARKOV_CHAIN ,
  SEQL_SEMI_MARKOV_CHAIN ,
  SEQL_HIDDEN_SEMI_MARKOV_CHAIN ,

  SEQL_MAX_ORDER ,
  SEQL_MEMORY_TREE ,
  SEQL_TRANSITION_TREE ,
  SEQL_MEMORY_TRANSITION_MATRIX ,
  SEQL_NON_TERMINAL ,
  SEQL_TERMINAL ,
  SEQL_COMPLETION ,
  SEQL_COMPLETED ,
  SEQL_TRANSITION_PROBABILITIY_CONFIDENCE_INTERVAL ,
  SEQL_FREE_TRANSIENT_PARAMETER ,
  SEQL_FREE_TRANSIENT_PARAMETERS ,
  SEQL_RECOMMENDED_MAX_ORDER ,
  SEQL_PRUNING_THRESHOLD ,
  SEQL_INITIAL_COUNTS ,
  SEQL_TRANSITION_COUNTS ,
  SEQL_MAX_TRANSITION_COUNT_DIFFERENCE ,
  SEQL_LIKELIHOODS ,
  SEQL_COUNT ,
  SEQL_DELTA ,
  SEQL_KRICHEVSKY_TROFIMOV ,

  SEQL_SELF_TRANSITION ,
  SEQL_ASYMPTOTE ,

  SEQL_NO_OCCURRENCE ,
  SEQL_FIRST_OCCURRENCE ,
  SEQL_FIRST_OCCURRENCE_OF ,
  SEQL_LEAVING ,
  SEQL_ABSORPTION ,
  SEQL_BIASED ,
  SEQL_OCCUPANCY ,
  SEQL_SOJOURN_TIME ,
  SEQL_INITIAL_RUN ,
  SEQL_FINAL_RUN ,
  SEQL_MIXTURE_OF ,
  SEQL_NB_RUN ,
  SEQL_NB_RUN_OF ,
  SEQL_NB_OCCURRENCE ,
  SEQL_NB_OCCURRENCE_OF ,
  SEQL_PER_SEQUENCE ,
  SEQL_PER_LENGTH ,
  SEQL_MISSING_VALUE ,
  SEQL_WORDS ,

  SEQL_STATE_PROBABILITY ,
  SEQL_POSTERIOR_STATE_SEQUENCE_PROBABILITY ,
  SEQL_STATE_BEGIN ,
  SEQL_POSTERIOR_STATE_PROBABILITY ,
  SEQL_POSTERIOR_IN_STATE_PROBABILITY ,
  SEQL_POSTERIOR_OUT_STATE_PROBABILITY ,
  SEQL_CONDITIONAL_ENTROPY ,
  SEQL_MARGINAL_ENTROPY ,
  SEQL_MARGINAL_ENTROPY_SUM ,
  SEQL_PARTIAL_STATE_SEQUENCE_ENTROPY ,
  SEQL_STATE_SEQUENCE_ENTROPY ,
  SEQL_UPPER_BOUND ,
  SEQL_NB_STATE_SEQUENCE ,
  SEQL_MAX_POSTERIOR_STATE_PROBABILITY ,
  SEQL_MAX_POSTERIOR_IN_STATE_PROBABILITY ,
  SEQL_MAX_POSTERIOR_OUT_STATE_PROBABILITY ,

  SEQL_CORRELATION_FUNCTION ,
  SEQL_PARTIAL ,
  SEQL_AUTO ,
  SEQL_CROSS ,
  SEQL_PEARSON ,
  SEQL_SPEARMAN ,
  SEQL_KENDALL ,
  SEQL_RANK ,
  SEQL_LAG ,
  SEQL_MAX_LAG ,
  SEQL_WHITE_NOISE ,
  SEQL_RANDOMNESS_95_CONFIDENCE_LIMIT ,
  SEQL_PAIR_FREQUENCY ,

  SEQL_INDEX ,

  SEQL_SIMULATED ,
  SEQL_SEQUENCE ,
  SEQL_SEQUENCES ,
  SEQL_VERTEX_IDENTIFIER ,
  SEQL_INDEX_PARAMETER ,
  SEQL_MIN_INDEX_PARAMETER ,
  SEQL_MAX_INDEX_PARAMETER ,
  SEQL_TIME ,
  SEQL_TIME_INTERVAL ,
  SEQL_POSITION ,
  SEQL_POSITION_INTERVAL ,
  SEQL_LENGTH ,
  SEQL_SEQUENCE_LENGTH ,
  SEQL_CUMUL_LENGTH ,
  SEQL_SHIFT ,

  SEQL_ALIGNMENT_LENGTH ,
  SEQL_ALIGNED_ON ,
  SEQL_MAX_GAP_LENGTH ,
  SEQL_ALIGNMENT_CODING ,
  SEQL_CONSENSUS ,

  SEQL_OPTIMAL ,
  SEQL_CHANGE_POINT ,
  SEQL_CHANGE_POINTS ,
  SEQL_SEGMENT ,
  SEQL_SEGMENTS ,
  SEQL_SEGMENT_MEAN ,
  SEQL_SEGMENT_VARIANCE ,
  SEQL_SEGMENT_STANDARD_DEVIATION ,
  SEQL_GLOBAL_STANDARD_DEVIATION ,
  SEQL_NB_SEGMENT ,
  SEQL_POSTERIOR_PROBABILITY ,
  SEQL_PENALTY ,
  SEQL_NB_SEGMENTATION ,
  SEQL_SEGMENTATIONS ,
  SEQL_SEGMENTATION_LIKELIHOOD ,
  SEQL_POSSIBLE_SEGMENTATION_LIKELIHOOD ,
  SEQL_POSTERIOR_CHANGE_POINT_PROBABILITY ,
  SEQL_POSTERIOR_SEGMENT_PROBABILITY ,
  SEQL_SEGMENTATION_ENTROPY ,
  SEQL_RANKED_CHANGE_POINT_ENTROPY ,
  SEQL_CHANGE_POINT_ENTROPY ,
  SEQL_UNIFORM_ENTROPY ,
  SEQL_SEGMENTATION_DIVERGENCE ,
  SEQL_MAX_CHANGE_POINT_LIKELIHOOD ,
  SEQL_MAX_SEGMENT_LIKELIHOOD ,
  SEQL_MAX_POSTERIOR_CHANGE_POINT_PROBABILITY ,
  SEQL_MAX_POSTERIOR_SEGMENT_PROBABILITY ,
  SEQL_AMBIGUITY ,

  SEQL_TOP ,
  SEQL_TOPS ,
  SEQL_NB_INTERNODE
};


extern const char *SEQ_label[];



/****************************************************************
 *
 *  Identificateurs des messages d'erreur pour l'analyse des fichiers :
 */


enum {
  SEQP_TIME_ORDER ,
  SEQP_MAX_TIME ,
  SEQP_NB_EVENT_ORDER ,

  SEQP_STATE ,
  SEQP_NB_MEMORY ,

  SEQP_TIME_INDEX_ORDER ,
  SEQP_POSITION_ORDER ,
  SEQP_POSITION ,
  SEQP_MAX_SEQUENCE_LENGTH
};


extern const char *SEQ_parsing[];



/****************************************************************
 *
 *  Identificateurs des messages d'erreur de traitement :
 */


enum {
  SEQR_BOTH_END_CENSORED_INTERVAL ,
  SEQR_INCOMPATIBLE_RENEWAL_DATA ,
  SEQR_MAX_NB_EVENT_TOO_SMALL ,
  SEQR_NB_EVENT_TOO_SMALL ,
  SEQR_TIME_UNIT ,
  SEQR_NB_COMPLETE_INTERVAL_TOO_SMALL ,
  SEQR_COMPLETE_MIN_VALUE ,
  SEQR_FORWARD_MIN_VALUE ,
  SEQR_NO_EVENT_MIN_VALUE ,
  SEQR_MEAN_COMPUTATION_METHOD ,
  SEQR_INTER_EVENT_SUPPORT ,

  SEQR_OBSERVATION_TIME ,
  SEQR_MIN_TIME ,
  SEQR_MAX_TIME ,
  SEQR_MIN_NB_EVENT ,
  SEQR_MAX_NB_EVENT ,
  SEQR_EMPTY_RENEWAL_DATA ,
  SEQR_SHORT_OBSERVATION_TIME ,
  SEQR_LONG_OBSERVATION_TIME ,

  SEQR_SINGLE_STATE_COMPONENT ,
  SEQR_NB_STATE ,
  SEQR_MISSING_STATE ,
  SEQR_ORDER ,
  SEQR_MIN_ORDER ,
  SEQR_MAX_ORDER ,
  SEQR_NB_PARAMETER ,
  SEQR_OVERLAP ,
  SEQR_MODEL_TYPE ,
  SEQR_VARIABLE_NB_VALUE ,
  SEQR_NB_OUTPUT_PROCESS ,
  SEQR_OUTPUT_PROCESS_INDEX ,
  SEQR_NB_OUTPUT ,
  SEQR_MIN_NB_STATE_SEQUENCE ,
  SEQR_NB_STATE_SEQUENCE ,
  SEQR_OCCUPANCY ,
  SEQR_NB_SEQUENCE ,
  SEQR_SEQUENCE_IDENTIFIER ,
  SEQR_SEQUENCE_IDENTIFIERS ,
  SEQR_REF_SEQUENCE_IDENTIFIER ,
  SEQR_TEST_SEQUENCE_IDENTIFIER ,
  SEQR_SEQUENCE_LENGTH ,
  SEQR_SHORT_SEQUENCE_LENGTH ,
  SEQR_LONG_SEQUENCE_LENGTH ,
  SEQR_CUMUL_SEQUENCE_LENGTH ,
  SEQR_STATES_NOT_REPRESENTED ,
  SEQR_STATE_SEQUENCE_COMPUTATION_FAILURE ,

  SEQR_SAMPLE_VERTEX_IDENTIFIER ,
  SEQR_VERTEX_IDENTIFIER ,
  SEQR_INDEX_PARAMETER_TYPE ,
  SEQR_INDEX_PARAMETER ,
  SEQR_POSITIVE_MIN_VALUE ,
  SEQR_DATE_ORDER ,
  SEQR_BEGIN_INDEX_PARAMETER ,
  SEQR_END_INDEX_PARAMETER ,
  SEQR_MIN_SEQUENCE_LENGTH ,
  SEQR_MAX_SEQUENCE_LENGTH ,
  SEQR_NOT_PRESENT ,
  SEQR_MAX_RUN_LENGTH ,
  SEQR_MIN_INDEX_PARAMETER ,
  SEQR_MAX_INDEX_PARAMETER ,
  SEQR_NB_SELECTED_VALUE ,
  SEQR_UNEQUAL_INDEX_INTERVALS ,
  SEQR_SINGLE_SEQUENCE ,
  SEQR_POSITION_STEP ,
  SEQR_LENGTH ,
  SEQR_VALUE ,
  SEQR_CORRELATION_COEFF_TYPE ,
  SEQR_FREQUENCY ,
  SEQR_MAX_LAG ,
  SEQR_INCOMPATIBLE_CORRELATION_FUNCTION ,
  SEQR_DIFFERENCING_ORDER ,
  SEQR_INITIAL_RUN_ALREADY_BUILT ,
  SEQR_RUN_LENGTH ,
  SEQR_MAX_NB_WORD ,
  SEQR_MIN_FREQUENCY ,

  SEQR_STATE_SEQUENCES ,
  SEQR_CHARACTERISTICS_NOT_COMPUTED ,
  SEQR_CONSECUTIVE_VALUES ,
  SEQR_NON_EXISTING_CHARACTERISTIC_DISTRIBUTION ,
  SEQR_NON_EXISTING_FORWARD_DISTRIBUTION ,
  SEQR_INCOMPATIBLE_MODEL ,
  SEQR_SEQUENCE_INCOMPATIBLE_MODEL ,

  SEQR_NB_ALIGNMENT ,
  SEQR_INDEL_FACTOR ,
  SEQR_TRANSPOSITION_FACTOR ,

  SEQR_FORBIDDEN_OUTPUT ,
  SEQR_NB_SEGMENT ,
  SEQR_MAX_NB_SEGMENT ,
  SEQR_CHANGE_POINT ,
  SEQR_SEGMENTATION_FAILURE ,
  SEQR_NB_SEGMENTATION ,
  SEQR_CHANGE_POINT_MODEL ,

  SEQR_POSITION ,
  SEQR_NB_INTERNODE ,
  SEQR_TOP_IDENTIFIER ,
  SEQR_MAIN_AXE_NB_INTERNODE ,
  SEQR_MIN_POSITION ,
  SEQR_MAX_POSITION ,
  SEQR_NEIGHBORHOOD ,
  SEQR_NEIGHBORS ,
  SEQR_EQUAL_PROBABILITY ,
  SEQR_NB_TOP ,
  SEQR_NB_TRIAL ,
  SEQR_NB_AXILLARY_SHOOT
};


extern const char *SEQ_error[];



#endif
