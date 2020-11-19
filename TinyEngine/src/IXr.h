#ifndef IXR_H_
#define IXR_H_

#include <cstdint>

class IXr
{

public:

	virtual ~IXr()
	{
	}

	virtual bool applicationInitXr()
	{
		return true;
	}

	virtual bool applicationUpdateXr(uint32_t frameIndex, double deltaTime, double totalTime)
	{
		return true;
	}

	virtual void applicationTerminateXr()
	{
	}

};

#endif /* IXR_H_ */
