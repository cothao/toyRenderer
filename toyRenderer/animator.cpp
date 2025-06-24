#include "animator.h"

Animator::Animator(std::shared_ptr<Animation> Animation)
{
	M_CurrentTime = 0.f;
	M_CurrentAnimation = Animation;

	M_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 300; i++) M_FinalBoneMatrices.push_back(glm::mat4(1.f));

}

void Animator::UpdateAnimation(float DeltaTime)
{

	M_DeltaTime = DeltaTime;

	if (M_CurrentAnimation)
	{
		M_CurrentTime += M_CurrentAnimation->GetTicksPerSecond() * DeltaTime;
		M_CurrentTime = fmod(M_CurrentTime, M_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&M_CurrentAnimation->GetRootNode(), glm::mat4(1.f));

	}
	else
	{
		std::cout << "Error with Animation: No Animation Found\n";
	}

}

void Animator::CalculateBoneTransform(const AssimpNodeData* Node, glm::mat4 ParentTransform)
{
	std::string NodeName = Node->Name;
	glm::mat4 NodeTransform = Node->Transformation;

	Bone* Bone = M_CurrentAnimation->FindBone(NodeName);

	if (Bone)
	{
		Bone->Update(M_CurrentTime);
		NodeTransform = Bone->GetLocalTransform();
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransform;

	auto BoneInfoMap = M_CurrentAnimation->GetBoneIDMap();
	if (BoneInfoMap.find(NodeName) != BoneInfoMap.end())
	{
		int Index = BoneInfoMap[NodeName].ID;
		glm::mat4 Offset = BoneInfoMap[NodeName].Offset;
		M_FinalBoneMatrices[Index] = GlobalTransformation * Offset;
	}

	for (int i = 0; i < Node->ChildrenCount; i++) CalculateBoneTransform(&Node->Children[i], GlobalTransformation);

	

}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
{
	return M_FinalBoneMatrices;
}

void Animator::PlayAnimation(std::shared_ptr<Animation> PAnimation)
{
	M_CurrentAnimation = PAnimation;
	M_CurrentTime = 0.0f;
}
