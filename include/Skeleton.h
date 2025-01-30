#include "core.h"
#include <vector>
#include "DOF.h"
#include "Joint.h"
#include <Tokenizer.h>

class Skeleton {
private:
	Joint* root;
public:
	void Load(const char* filename);
	void Update();
	void Draw();
	~Skeleton();

	static std::vector<Joint*> joints;
	static std::vector<DOF*> DOFs;
};
