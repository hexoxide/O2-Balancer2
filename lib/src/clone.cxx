#include "clone.h"

std::unique_ptr<CloneAble> CloneAble::clone()
{ 
	return std::unique_ptr<CloneAble>(doClone());
}

CloneAble* CloneAble::doClone()
{ 
	return new CloneAble(*this);
}

CloneAble::~CloneAble()
{
	
}