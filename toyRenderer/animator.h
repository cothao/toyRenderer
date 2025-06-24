#ifndef ANIMATOR_H
#define ANIMATOR_H
#include "animation.h"

class Animator
{

public:
	Animator() = default;
	Animator(std::shared_ptr<Animation> Animation);

	void UpdateAnimation(float DeltaTime);
	void PlayAnimation(std::shared_ptr<Animation> PAnimation);
	void CalculateBoneTransform(const AssimpNodeData* Node, glm::mat4 ParentTransform);
	std::vector<glm::mat4> GetFinalBoneMatrices();
	float M_CurrentTime;

protected:
	friend class Actor;
	std::vector<glm::mat4> M_FinalBoneMatrices;
	std::shared_ptr<Animation> M_CurrentAnimation;
	float M_DeltaTime;

};

#endif