#include "PathNode.h"



UPathNode::UPathNode()
{
	Position = FVector::ZeroVector;
	blocked = false;
	PathMultiplier = 1.0f;
}