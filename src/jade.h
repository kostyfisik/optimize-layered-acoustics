#ifndef SRC_JADE_H_
#define SRC_JADE_H_
///
/// @file   jade.h
/// @author Ladutenko Konstantin <kostyfisik at gmail (.) com>
/// @date   Thu Aug 15 19:21:57 2013
/// @copyright 2013 Ladutenko Konstantin
/// @section LICENSE
/// This file is part of JADE++.
///
/// JADE++ is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// JADE++ is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with JADE++.  If not, see <http://www.gnu.org/licenses/>.

/// @brief JADE++ is a free (GPLv3+) high performance implementation of
/// adaptive differential evolution optimization algorithm from
/// Jingqiao Zhang and Arthur C. Sanderson book 'Adaptive Differential
/// Evolution. A Robust Approach to Multimodal Problem Optimization'
/// Springer, 2009. Crossover rate was patched according to PMCRADE
/// approach supposed by Jie Li, Wujie Zhu, Mengjun Zhou, and Hua Wang
/// in 'Power Mean Based Crossover Rate Adaptive Differential
/// Evolution' in H. Deng et al. (Eds.): AICI 2011, Part II, LNAI
/// 7003, pp. 34–41, 2011

#include <random>
#include <utility>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>
namespace jade {
  // ********************************************************************** //
  // ********************************************************************** //
  // ********************************************************************** //
  /// @brief Population controlled by single MPI process.
  class SubPopulation {
   public:
    /// @brief Externaly defined fitness function, used by pointer.
    double (*FitnessFunction)(std::vector<double> x) = nullptr;
    /// @brief Class initialization.
    int Init(long total_population, long dimension);              // NOLINT
    /// @brief Vizualize used random distributions (to do manual check).
    void SetFeed(std::vector<std::vector<double> > x_feed_vectors);
    void CheckRandom();
    /// @brief Find optimum value of fitness function.
    int RunOptimization();
    /// @brief Set maximum number of generations used for optimization.
    void SetTotalGenerationsMax(long gen) {total_generations_max_ = gen;} // NOLINT
    /// @brief Select if to find global minimum or maximum of fitness function.
    void SetTargetToMinimum() {is_find_minimum_ = true;}
    void SetTargetToMaximum() {is_find_minimum_ = false;}
    /// @brief Set adaption parameters.
    int SetBestShareP(double p);
    int SetAdapitonFrequencyC(double c);
    /// @brief Set level of algorithm distribution.
    /// 0 - no distribution, each MPI process acts independantly.
    int SetDistributionLevel(int level);
    /// @brief Set same search bounds for all components of fitness
    /// function input vector.
    int SetAllBounds(double lbound, double ubound);
    void SetAllBoundsVectors
        (std::vector<double> lbound, std::vector<double> ubound);
    /// @brief Print Optimization parameters.
    int PrintParameters(std::string comment);
    /// @brief Print final result
    int PrintResult(std::string comment);
    std::vector<double> GetFinalFitness();
    std::vector<double> GetBest(double *best_fitness);
    std::vector<double> GetWorst(double *worst_fitness);
    int ErrorStatus() {return error_status_;};
    void SwitchOffPMCRADE(){isPMCRADE_ = false;};
   private:
    bool isPMCRADE_ = true;
    bool isFeed_ = false;
    int CreateInitialPopulation();
    int PrintPopulation();
    int PrintEvaluated();
    int PrintSingleVector(std::vector<double> x);
    int SortEvaluatedCurrent();
    /// @brief Apply fitness function to current population.
    int EvaluateCurrentVectors();
    /// @brief Generate crossover and mutation factors for current individual
    int SetCRiFi(long i);
    /// @name Main algorithm steps.
    // @{
    int Selection(std::vector<double> crossover_u,
                  long individual_index);
    int ArchiveCleanUp();
    int Adaption();
    std::vector<double> Mutation(long individual_index);
    std::vector<double> Crossover(std::vector<double> mutated_v,
                                  long individual_index);
    // @}
    /// @name Other algorithm steps.
    // @{
    std::vector<double> GetXpBestCurrent();
    /// @brief Returns random vector from current population and
    /// vector`s index.
    std::vector<double> GetXRandomCurrent(long *index,
                                          long forbidden_index);
    std::vector<double> GetXRandomArchiveAndCurrent(
                   long forbidden_index1, long forbidden_index2);
    // @}
    /// @name Population, individuals and algorithm .
    // @{
    /// @brief Search minimum or maximum of fitness function.
    bool is_find_minimum_ = true;
    /// @brief Maximum number of generations used for optimization.
    long total_generations_max_ = 0;                                   // NOLINT
    /// @brief Total number of individuals in all subpopulations.
    long total_population_ = 0;                                        // NOLINT
    /// @brief Number of individuals in subpopulation
    long subpopulation_ = 0;                                  // NOLINT
    /* /// @brief All individuals are indexed. First and last index of */
    /* /// individuals in subpopulations. */
    /* long index_first_ = -1, index_last_ = -1;                          // NOLINT */
    /// @brief Dimension of the optimization task (number of variables
    /// to optimize).
    long dimension_ = -1;                                              // NOLINT
    /// @brief Current generation of evalution process;
    long current_generation_ = -1;                                     // NOLINT
    /// @brief Several feed vectors.
    std::vector<std::vector<double> > x_feed_vectors_;
    /// @brief Current state vectors of all individuals in subpopulation.
    std::vector<std::vector<double> > x_vectors_current_;
    /// @brief State vectors of all individuals in subpopulation in
    /// new generation.
    std::vector<std::vector<double> > x_vectors_next_generation_;
    /// @brief Sometimes sorted list of evaluated fitness function.
    std::list<std::pair<double, long> >                                // NOLINT
        evaluated_fitness_for_current_vectors_;
    /// @brief Sometimes sorted list of evaluated fitness function for
    /// next generation.
    std::list<std::pair<double, long> >                                // NOLINT
        evaluated_fitness_for_next_generation_;
    /// @brief Archived best solutions (state vactors)
    std::list<std::vector<double> > archived_best_A_;
    std::list<std::vector<double> > to_be_archived_best_A_;
    /// @brief Sometimes sorted list of evaluated fitness function for
    /// best vectors.
    std::list<std::pair<double, long> >                                // NOLINT
        evaluated_fitness_for_archived_best_;
    /// @brief Low and upper bounds for x vectors.
    std::vector<double> x_lbound_;
    std::vector<double> x_ubound_;
    /// @brief JADE+ adaption parameter for mutation factor
    double adaptor_mutation_mu_F_ = 0.5;
    /// @brief JADE+ adaption parameter for crossover probability
    double adaptor_crossover_mu_CR_ = 0.5;
    /// @brief Individual mutation and crossover parameters for each individual.
    std::vector<double> mutation_F_, crossover_CR_;
    std::list<double> successful_mutation_parameters_S_F_;
    std::list<double> successful_crossover_parameters_S_CR_;
    /// @brief Share of all individuals in current population to be
    /// the best, recomended value range 0.05-0.2
    //const double best_share_p_ = 0.12;
    double best_share_p_ = 0.05;

    /* //debug Change it back before production!!!! */
    /* const double best_share_p_ = 0.3; */

    /// @brief 1/c - number of generations accounted for parameter
    /// adaption, recommended value 5 to 20 generation;
    //const double adaptation_frequency_c_ = 1.0/20.0;    
    double adaptation_frequency_c_ = 0.1;    
    // @}
    /// @name Random generation
    /// Names are in notation from Jingqiao Zhang and Arthur C. Sanderson book.
    // @{
    /// @todo Select random generator enginge for best results in DE!
    std::mt19937_64 generator_;
    //std::ranlux48 generator_;
    /// @brief randn(&mu;, &sigma^2; ) denotes a random value from a normal
    /// distribution of mean &mu; and variance &sigma^2;
    double randn(double mean, double stddev);
    /// @brief randc(&mu;, &delta; ) a random value from a Cauchy distribution
    /// with location and scale parameters &mu; and &delta;
    double randc(double location, double scale);
    /// @brief randint(1, D) is an integer randomly chosen from 1 to D
    long randint(long lbound, long ubound);                  // NOLINT
    /// @brief rand(a, b) is an uniform random number chosen from a to b
    double rand(double lbound, double ubound);                              // NOLINT
    // @}
    /// @name MPI section
    // @{
    int process_rank_;
    int number_of_processes_;
    int AllGatherVectorDouble(std::vector<double> to_send);
    std::vector<double> recieve_double_;
    int AllGatherVectorLong(std::vector<long> to_send);
    std::vector<long> recieve_long_;
       
    // @}
    /// @brief Subpopulation status. If non-zero than some error has appeared.
    int error_status_ = 0;
    int distribution_level_ = 0;
  };  // end of class SubPopulation
  // ********************************************************************** //
  // ********************************************************************** //
  // ********************************************************************** //
  /// @brief Error codes
  ///
  /// Error codes used with jade
  enum Errors {
    /// no error
    kDone = 0,
    /// Unspecified (pending to be described).
    kError
  };  // end of enum Errors
  // ********************************************************************** //
  // ********************************************************************** //
  // ********************************************************************** //
  const int kOutput = 0; /// Process rank to do output with printf
  template<class T> inline T pow2(const T value) {return value*value;}
}  // end of namespace jade
#endif  // SRC_JADE_H_
