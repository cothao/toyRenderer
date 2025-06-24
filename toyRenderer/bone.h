#ifndef BONE_H
#define BONE_H
#include "glm/glm.hpp"
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>
#include "assimp/anim.h"
#include "assimpglmhelpers.h"

struct KeyPosition
{
	glm::vec3 Position;
	float TimeStamp;
};

struct KeyRotation
{
	glm::quat Orientation;
	float TimeStamp;
};

struct KeyScale
{
	glm::vec3 Scale;
	float TimeStamp;
};

class Bone
{

private:
	std::vector<KeyPosition>	M_Positions;
	std::vector<KeyRotation>	M_Rotations;
	std::vector<KeyScale>		M_Scales;
	int M_NumPositions;
	int M_NumRotations;
	int M_NumScalings;

	glm::mat4 M_LocalTransform;
	std::string M_Name;
	int M_ID;

public:
	Bone(const std::string& Name, int ID, const aiNodeAnim* Channel);

	void		Update(float AnimationTime);
	int			GetPositionIndex(float AnimationTime);
	int			GetRotationIndex(float AnimationTime);
	int			GetScaleIndex(float AnimationTime);

	glm::mat4	GetLocalTransform() const { return M_LocalTransform; };
	std::string	GetBoneName() const { return M_Name; };
	int			GetBoneID() const { return M_ID; };

private:
	float GetScaleFactor(float LastTimeStamp, float NextTimeStamp, float AnimationTime);
	glm::mat4 InterpolatePosition(float AnimationTime);
	glm::mat4 InterpolateRotation(float AnimationTime);
	glm::mat4 InterpolateScaling(float AnimationTime);

};

#endif // !BONE_H
