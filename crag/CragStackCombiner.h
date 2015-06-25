#ifndef CANDIDATE_MC_CRAG_CRAG_STACK_COMBINER_H__
#define CANDIDATE_MC_CRAG_CRAG_STACK_COMBINER_H__

#include <vector>
#include "Crag.h"

/**
 * Combines a stack of crags (coming from a stack of images) into a single crag.
 */
class CragStackCombiner {

public:

	CragStackCombiner();

	void combine(const std::vector<Crag>& crags, Crag& crag);

private:

	std::map<Crag::Node, Crag::Node> copyNodes(const Crag& source, Crag& target);

	std::vector<std::pair<Crag::Node, Crag::Node>> findLinks(const Crag& a, const Crag& b);

	double _maxDistance;

	bool _requireBbOverlap;
};

#endif // CANDIDATE_MC_CRAG_CRAG_STACK_COMBINER_H__
