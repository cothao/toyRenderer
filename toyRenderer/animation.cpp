#include "animation.h"

Animation::Animation(const std::string& AnimationPath, std::shared_ptr<Model> Model_, int AnimationIndex)
{
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(AnimationPath, aiProcess_Triangulate);
	assert(Scene && Scene->mRootNode);
	auto Animation = Scene->mAnimations[AnimationIndex];
	std::cout << Scene->mAnimations;
	M_Duration = Animation->mDuration;
	M_TicksPerSecond = Animation->mTicksPerSecond;
	aiMatrix4x4 globalTransformation = Scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();
	ReadHeirarchyData(M_RootNode, Scene->mRootNode);
	ReadMissingBones(Animation, *Model_);
};

Animation::~Animation() {};

Bone* Animation::FindBone(const std::string& Name)
{
	auto Iter = std::find_if(M_Bones.begin(), M_Bones.end(),
		[&](const Bone& Bone)
		{
			return Bone.GetBoneName() == Name;
		}
	);
	if (Iter == M_Bones.end()) return nullptr;
	else return &(*Iter);
};

void Animation::ReadMissingBones(const aiAnimation* Animation, Model& Model_)
{
	int Size = Animation->mNumChannels;

	auto& BoneInfoMap = Model_.GetBoneInfoMap();
	int& BoneCount = Model_.GetBoneCount();

	for (int i = 0; i < Size; i++)
	{
	
		auto Channel = Animation->mChannels[i];
		std::string BoneName = Channel->mNodeName.data;

		if (BoneInfoMap.find(BoneName) == BoneInfoMap.end())
		{
			BoneInfoMap[BoneName].ID = BoneCount;
			BoneCount++;
		}

		M_Bones.push_back(Bone(Channel->mNodeName.data, BoneInfoMap[Channel->mNodeName.data].ID, Channel));

	}

	M_BoneInfoMap = BoneInfoMap;

};

void Animation::ReadHeirarchyData(AssimpNodeData& Dest, const aiNode* Src)
{

	assert(Src);

	Dest.Name = Src->mName.data;
	Dest.Transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(Src->mTransformation);
	Dest.ChildrenCount = Src->mNumChildren;

	for (int i = 0; i < Src->mNumChildren; i++)
	{
		AssimpNodeData NewData;
		ReadHeirarchyData(NewData, Src->mChildren[i]);
		Dest.Children.push_back(NewData);
	}

};