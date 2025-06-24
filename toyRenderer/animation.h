#ifndef ANIMATION_H
#define ANIMATION_H
#include "glm/glm.hpp"
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>
#include "assimp/anim.h"
#include "assimpglmhelpers.h"
#include "model.h"
#include "bone.h"
#include <memory>

struct AssimpNodeData
{
	glm::mat4 Transformation;
	std::string Name;
	int ChildrenCount;
	std::vector<AssimpNodeData> Children;

};

class Animation
{

private:
	float M_Duration;
	int M_TicksPerSecond;
	std::vector<Bone> M_Bones;
	AssimpNodeData M_RootNode;
	std::map<std::string, BoneInfo> M_BoneInfoMap;

public:
	Animation() = default;
	Animation(const std::string& AnimationPath, std::shared_ptr<Model> Model_, int AnimationIndex);

	~Animation();

	Bone* FindBone(const std::string& Name);
	inline float GetTicksPerSecond() { return M_TicksPerSecond; };
	inline float GetDuration() { return M_Duration; };
	inline const AssimpNodeData& GetRootNode() { return M_RootNode; };
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap() { return M_BoneInfoMap; };

private:
	void ReadMissingBones(const aiAnimation* Animation, Model& Model_);
	void ReadHeirarchyData(AssimpNodeData& Dest, const aiNode* Src);
};

#endif