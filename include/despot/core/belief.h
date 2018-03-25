#ifndef BELIEF_H
#define BELIEF_H

#include <vector>

#include <despot/util/random.h>
#include <despot/util/logging.h>
#include <despot/core/history.h>

namespace despot {

class State;
class StateIndexer;
class DSPOMDP;

/* =============================================================================
 * Belief class
 * =============================================================================*/

class Belief {
public:
	const DSPOMDP* model_;
	History history_;

public:
	Belief(const DSPOMDP* model);
	virtual ~Belief();

	virtual std::vector<State*> Sample(int num) const = 0;

  /*
   * Update the history by adding the action a and the observation o pair.
   * Update the particles by stepping each particle with the action
   * with randomly sampled observations o'.
   * Then compute the prob of o and assign the prob to weigh the particle.
   * Now we have a newly weighted and filtered list of particles which support
   * the given a and o.
   *
   * If the list is empty, we resample.
   * Normalize the weights of the particles in the new list.
   */
	virtual void Update(int action, OBS_TYPE obs) = 0;

	virtual std::string text() const;
	friend std::ostream& operator<<(std::ostream& os, const Belief& x);
	virtual Belief* MakeCopy() const = 0;

	static std::vector<State*> Sample(
      int num, std::vector<State*> belief, const DSPOMDP* model);

  /*
   * Sample a particle, step through the actions in the history,
   * use random number to sample observations.
   * If prob is good, save it.
   * Remove all the particles with small weights.
   * Keep generating new particles until we have enough amount.
   */
  /*
   * I. Resampling by searching initial particles
   * which are consistent with history.
   */
	static std::vector<State*> Resample(
      int num, const std::vector<State*>& belief,
      const DSPOMDP* model, History history, int hstart = 0);
  /*
   * II. Resampling by drawing random particles from prior
   * which are consistent with history.
   */
	static std::vector<State*> Resample(
      int num, const Belief& belief, History history, int hstart = 0);

  /*
   * Resampling by searching states consistent
   * with last (action, observation) pair.
   */
	static std::vector<State*> Resample(
      int num, const DSPOMDP* model,
      const StateIndexer* indexer, int action, OBS_TYPE obs);
};

/* =============================================================================
 * ParticleBelief class
 * =============================================================================*/

class ParticleBelief: public Belief {
protected:
	std::vector<State*> particles_;
	int num_particles_;
	Belief* prior_;
	bool split_;
	std::vector<State*> initial_particles_;
	const StateIndexer* state_indexer_;

public:
	ParticleBelief(std::vector<State*> particles, const DSPOMDP* model,
		Belief* prior = NULL, bool split = true);

	virtual ~ParticleBelief();
	void state_indexer(const StateIndexer* indexer);

	virtual const std::vector<State*>& particles() const;
	virtual std::vector<State*> Sample(int num) const;

	virtual void Update(int action, OBS_TYPE obs);

	virtual Belief* MakeCopy() const;

	virtual std::string text() const;
};

} // namespace despot

#endif
