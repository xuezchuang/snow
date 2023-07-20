// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Written by Michael Zeilfelder

#pragma once

#include "IrrCompileConfig.h"
#include "IProfiler.h"

namespace irr
{
class CProfiler : public IProfiler
{
public:

	CProfiler();
	virtual ~CProfiler();

	//! Write all profile-data into a string
	virtual void printAll(core::stringw& result, bool includeOverview, bool suppressUncalled) const  _IRR_OVERRIDE_;

	//! Write the profile data of one group into a string
	virtual void printGroup(core::stringw& result, u32 groupIndex, bool suppressUncalled) const  _IRR_OVERRIDE_;

protected:
	core::stringw makeTitleString() const;
	core::stringw getAsString(const SProfileData& data) const;
};
} // namespace irr

