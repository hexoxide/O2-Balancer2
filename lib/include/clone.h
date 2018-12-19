#include <memory>

class CloneAble {
    public:
    	std::unique_ptr<CloneAble> clone();
    	virtual ~CloneAble();
    private:
    	virtual CloneAble* doClone();
};