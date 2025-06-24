#include "bone.h"

Bone::Bone(const std::string& Name, int ID, const aiNodeAnim* Channel)
	:M_Name(Name), M_ID(ID), M_LocalTransform(1.f)
{

	M_NumPositions = Channel->mNumPositionKeys;

	for (int PositionIndex = 0; PositionIndex < M_NumPositions; ++PositionIndex)
	{
		aiVector3D aiPosition = Channel->mPositionKeys[PositionIndex].mValue;
		float TimeStamp = Channel->mPositionKeys[PositionIndex].mTime;
		KeyPosition Data;
		Data.Position = AssimpGLMHelpers::GetGLMVec(aiPosition);
		Data.TimeStamp = TimeStamp;
		M_Positions.push_back(Data);
	}	
	
	M_NumRotations = Channel->mNumRotationKeys;

	for (int RotationIndex = 0; RotationIndex < M_NumRotations; ++RotationIndex)
	{
		aiQuaternion aiOrientation = Channel->mRotationKeys[RotationIndex].mValue;
		float TimeStamp = Channel->mRotationKeys[RotationIndex].mTime;
		KeyRotation Data;
		Data.Orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
		Data.TimeStamp = TimeStamp;
		M_Rotations.push_back(Data);
	}	
	
	M_NumScalings = Channel->mNumScalingKeys;

	for (int KeyIndex = 0; KeyIndex < M_NumScalings; ++KeyIndex)
	{
		aiVector3D Scale = Channel->mScalingKeys[KeyIndex].mValue;
		float TimeStamp = Channel->mScalingKeys[KeyIndex].mTime;
		KeyScale Data;
		Data.Scale = AssimpGLMHelpers::GetGLMVec(Scale);
		Data.TimeStamp = TimeStamp;
		M_Scales.push_back(Data);
	}

}

void Bone::Update(float AnimationTime)
{
	glm::mat4 Translation = InterpolatePosition(AnimationTime);
	glm::mat4 Rotation = InterpolateRotation(AnimationTime);
	glm::mat4 Scale = InterpolateScaling(AnimationTime);
	M_LocalTransform = Translation * Rotation * Scale;
}

int Bone::GetPositionIndex(float AnimationTime)
{
	for (int Index = 0; Index < M_NumPositions - 1; ++Index)
	{
		if (AnimationTime < M_Positions[Index + 1].TimeStamp) return Index;
	}
	assert(0);
}

int Bone::GetRotationIndex(float AnimationTime)
{
	for (int Index = 0; Index < M_NumRotations - 1; ++Index)
	{
		if (AnimationTime < M_Rotations[Index + 1].TimeStamp) return Index;
	}
	assert(0);
}

int Bone::GetScaleIndex(float AnimationTime)
{
	for (int Index = 0; Index < M_NumScalings - 1; ++Index)
	{
		if (AnimationTime < M_Scales[Index + 1].TimeStamp) return Index;
	}
	assert(0);
}

float Bone::GetScaleFactor(float LastTimeStamp, float NextTimeStamp, float AnimationTime)
{

	float ScaleFactor = 0.f;
	float MidWayLength = AnimationTime - LastTimeStamp;
	float FramesDiff = NextTimeStamp - LastTimeStamp;
	ScaleFactor = MidWayLength/FramesDiff;
	return ScaleFactor;

}

glm::mat4 Bone::InterpolatePosition(float AnimationTime)
{

	if (1 == M_NumPositions) return glm::translate(glm::mat4(1.f), M_Positions[0].Position);

	int P0Index = GetPositionIndex(AnimationTime);
	int P1Index = P0Index + 1;
	float ScaleFactor = GetScaleFactor(M_Positions[P0Index].TimeStamp, M_Positions[P1Index].TimeStamp, AnimationTime);
	glm::vec3 FinalPosition = glm::mix(M_Positions[P0Index].Position, M_Positions[P1Index].Position, ScaleFactor);
	return glm::translate(glm::mat4(1.f), FinalPosition);
};

glm::mat4 Bone::InterpolateRotation(float AnimationTime)
{

	if (1 == M_NumRotations)
	{
		auto Rotation = glm::normalize(M_Rotations[0].Orientation);
		return glm::toMat4(Rotation);
	}

	int P0Index = GetRotationIndex(AnimationTime);
	int P1Index = P0Index + 1;
	float ScaleFactor = GetScaleFactor(M_Rotations[P0Index].TimeStamp, M_Rotations[P1Index].TimeStamp, AnimationTime);
	glm::quat FinalRotation = glm::slerp(M_Rotations[P0Index].Orientation, M_Rotations[P1Index].Orientation, ScaleFactor);
	
	FinalRotation = glm::normalize(FinalRotation);
	
	return glm::toMat4(FinalRotation);
};

glm::mat4 Bone::InterpolateScaling(float AnimationTime)
{

	if (1 == M_NumScalings) return glm::scale(glm::mat4(1.f), M_Scales[0].Scale);

	int P0Index = GetScaleIndex(AnimationTime);
	int P1Index = P0Index + 1;
	float ScaleFactor = GetScaleFactor(M_Scales[P0Index].TimeStamp, M_Scales[P1Index].TimeStamp, AnimationTime);
	glm::vec3 FinalScale = glm::mix(M_Scales[P0Index].Scale, M_Scales[P1Index].Scale, ScaleFactor);
	return glm::scale(glm::mat4(1.f), FinalScale);
};
