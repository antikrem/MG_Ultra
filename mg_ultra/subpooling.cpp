#include "subpooling.h"

bool operator==(const SubPoolComponents& lhs, const SubPoolComponents& rhs) {
	return
		lhs.requiredTypes == rhs.requiredTypes;
}

bool operator==(const SubPoolTarget& lhs, const SubPoolTarget& rhs) {
	return
		lhs.cachedTarget == rhs.cachedTarget 
		&& lhs.targets == rhs.targets;
}