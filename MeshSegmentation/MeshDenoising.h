
// MeshDenoising.h

#include "glm.h"


class MeshDenoising
{
public:
	MeshDenoising();
	~MeshDenoising();

	void BilateralDenoising(GLMmodel *meshmodel);

protected:
private:
};
