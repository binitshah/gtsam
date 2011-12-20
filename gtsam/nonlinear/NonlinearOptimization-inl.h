/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file NonlinearOptimization-inl.h
 * @date Oct 17, 2010
 * @author Kai Ni
 * @brief Easy interfaces for NonlinearOptimizer
 */

#pragma once

#include <gtsam/linear/GaussianSequentialSolver.h>
#include <gtsam/linear/GaussianMultifrontalSolver.h>

#include <gtsam/linear/SubgraphSolver.h>
#include <gtsam/nonlinear/NonlinearOptimizer.h>

using namespace std;

namespace gtsam {

	/**
	 * The Elimination solver
	 */
	template<class G, class T>
	T	optimizeSequential(const G& graph, const T& initialEstimate,
			const NonlinearOptimizationParameters& parameters, bool useLM) {

		// Use a variable ordering from COLAMD
	  Ordering::shared_ptr ordering = graph.orderingCOLAMD(initialEstimate);

		// Create an nonlinear Optimizer that uses a Sequential Solver
	  typedef NonlinearOptimizer<G, T, GaussianFactorGraph, GaussianSequentialSolver> Optimizer;
	  Optimizer optimizer(boost::make_shared<const G>(graph),
	  		boost::make_shared<const T>(initialEstimate), ordering,
	  		boost::make_shared<NonlinearOptimizationParameters>(parameters));

	  // Now optimize using either LM or GN methods.
		if (useLM)
			return *optimizer.levenbergMarquardt().values();
		else
			return *optimizer.gaussNewton().values();
	}

	/**
	 * The multifrontal solver
	 */
	template<class G, class T>
	T	optimizeMultiFrontal(const G& graph, const T& initialEstimate,
			const NonlinearOptimizationParameters& parameters, bool useLM) {

		// Use a variable ordering from COLAMD
	  Ordering::shared_ptr ordering = graph.orderingCOLAMD(initialEstimate);

		// Create an nonlinear Optimizer that uses a Multifrontal Solver
	  typedef NonlinearOptimizer<G, T, GaussianFactorGraph, GaussianMultifrontalSolver> Optimizer;
	  Optimizer optimizer(boost::make_shared<const G>(graph),
	  		boost::make_shared<const T>(initialEstimate), ordering,
	  		boost::make_shared<NonlinearOptimizationParameters>(parameters));

	  // now optimize using either LM or GN methods
		if (useLM)
			return *optimizer.levenbergMarquardt().values();
		else
			return *optimizer.gaussNewton().values();
	}

	/**
	 * The sparse preconditioned conjugate gradient solver
	 */
	template<class G, class T>
	T	optimizeSPCG(const G& graph, const T& initialEstimate,
			const NonlinearOptimizationParameters& parameters = NonlinearOptimizationParameters(),
			bool useLM = true) {

		// initial optimization state is the same in both cases tested
		typedef SubgraphSolver<G,GaussianFactorGraph,T> Solver;
		typedef boost::shared_ptr<Solver> shared_Solver;
		typedef NonlinearOptimizer<G, T, GaussianFactorGraph, Solver> SPCGOptimizer;
		shared_Solver solver = boost::make_shared<Solver>(
				graph, initialEstimate, IterativeOptimizationParameters());
		SPCGOptimizer optimizer(
				boost::make_shared<const G>(graph),
				boost::make_shared<const T>(initialEstimate),
				solver->ordering(),
				solver,
				boost::make_shared<NonlinearOptimizationParameters>(parameters));

	  // choose nonlinear optimization method
		if (useLM)
			return *optimizer.levenbergMarquardt().values();
		else
			return *optimizer.gaussNewton().values();
	}

	/**
	 * optimization that returns the values
	 */
	template<class G, class T>
	T optimize(const G& graph, const T& initialEstimate, const NonlinearOptimizationParameters& parameters,
			const LinearSolver& solver,
			const NonlinearOptimizationMethod& nonlinear_method) {
		switch (solver) {
		case SEQUENTIAL:
			return optimizeSequential<G,T>(graph, initialEstimate, parameters,
					nonlinear_method == LM);
		case MULTIFRONTAL:
			return optimizeMultiFrontal<G,T>(graph, initialEstimate, parameters,
					nonlinear_method == LM);
		case SPCG:
//			return optimizeSPCG<G,T>(graph, initialEstimate, parameters,
//								nonlinear_method == LM);
			throw runtime_error("optimize: SPCG not supported yet due to the specific pose constraint");
		}
		throw runtime_error("optimize: undefined solver");
	}

} //namespace gtsam
