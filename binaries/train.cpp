/**
 * Reads a treemc project file containing features and a ground-truth labelling 
 * and trains node and edge feature weights.
 */

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#include <util/Logger.h>
#include <util/ProgramOptions.h>
#include <util/exceptions.h>
#include <io/Hdf5CragStore.h>
#include <io/Hdf5VolumeStore.h>
#include <learning/BundleOptimizer.h>
#include <learning/Oracle.h>
#include <learning/OverlapCosts.h>
#include <learning/BestEffort.h>
#include <learning/HammingLoss.h>

util::ProgramOption optionProjectFile(
		util::_long_name        = "projectFile",
		util::_short_name       = "p",
		util::_description_text = "The treemc project file.",
		util::_default_value    = "project.hdf");

util::ProgramOption optionFeatureWeights(
		util::_long_name        = "featureWeights",
		util::_short_name       = "w",
		util::_description_text = "A file to store the learnt feature weights.",
		util::_default_value    = "feature_weights.txt");

int main(int argc, char** argv) {

	try {

		util::ProgramOptions::init(argc, argv);
		logger::LogManager::init();

		Hdf5CragStore   cragStore(optionProjectFile.as<std::string>());
		Hdf5VolumeStore volumeStore(optionProjectFile.as<std::string>());

		Crag crag;
		cragStore.retrieveCrag(crag);

		NodeFeatures nodeFeatures(crag);
		EdgeFeatures edgeFeatures(crag);

		cragStore.retrieveNodeFeatures(crag, nodeFeatures);
		cragStore.retrieveEdgeFeatures(crag, edgeFeatures);

		BundleOptimizer::Parameters parameters;
		parameters.epsStrategy = BundleOptimizer::EpsFromGap;
		BundleOptimizer optimizer(parameters);

		ExplicitVolume<int> groundTruth;
		volumeStore.retrieveGroundTruth(groundTruth);
		OverlapCosts overlapCosts(crag, groundTruth);
		BestEffort   bestEffort(crag, overlapCosts);
		HammingLoss  loss(crag, bestEffort);

		Crag::NodeIt n(crag);
		bestEffort.node[n] = true;

		Oracle oracle(
				crag,
				nodeFeatures,
				edgeFeatures,
				loss,
				bestEffort);

		std::vector<double> weights(nodeFeatures.dims() + edgeFeatures.dims(), 0);
		optimizer.optimize(oracle, weights);

		std::ofstream weightsFile(optionFeatureWeights.as<std::string>());
		for (double f : weights)
			weightsFile << f << std::endl;

	} catch (boost::exception& e) {

		handleException(e, std::cerr);
	}
}

