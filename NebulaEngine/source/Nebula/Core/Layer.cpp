#include "Layer.h"

namespace Nebula
{

	Layer::Layer(const std::string& debugName)
		: m_DebugName(debugName)
	{
		NB_CORE_DEBUG("Layer created: {0}", m_DebugName);
	}

	Layer::~Layer()
	{
		NB_CORE_DEBUG("Layer destroyed: {0}", m_DebugName);
	}

}