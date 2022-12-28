#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <utility>
#include <stdlib.h> 
#include <cmath>

enum TheLabel     { Label };
enum TreeChlidren { Yes, No, NumOfChildren };

/* Using typedef to create alternate names for the datatypes we are using */
typedef std::vector<std::vector<std::string>>         Vec_vec_str;
typedef std::vector<std::string>                      Vec_str;
typedef std::vector<std::size_t>                      Vec_sz_t;
typedef std::pair<std::pair<std::string, float>, int> Pair_PairStrFloat_Int;
typedef std::pair<std::string, int>                   Pair_Str_Int;
typedef std::set<std::string>                         Set_str;
typedef std::map<std::string, std::size_t>            Map_Str_Sz_t;
typedef std::set<std::size_t>                         Set_Sz_t;

class DTree
{
	private:
		class Question
		{
			public:
				std::string  Q;
				Pair_Str_Int QInfo;

				inline Question() noexcept;                                       // Def Constructor
				inline Question(const std::string &newQ, 
					const Pair_Str_Int &newQInfo) noexcept;                   	  // Constructor
				inline const Question &operator=(const Question &other) noexcept; // operator = 
		};	// Question

		class Node
		{
			public:
				bool                Leaf;
				Vec_vec_str         Data;
				std::vector<Node *> Children;
				float               Impurity;
				Question            Qst;

				inline Node(const Vec_vec_str &newData,
					bool isLeaf, float newImpurity) noexcept;   // Constructor
				inline void Write() const noexcept;             // Write
		};	// Node

		//---Private Methods---//
		inline Pair_Str_Int
			FindTheRightQuestion(const Vec_vec_str &Set) const noexcept; 	   // FindTheRightQuestion
		inline float 
			CalculateImpurity(const Vec_vec_str &DataSet) const noexcept;  	   // CalculateImpurity
		inline void BuildTree(Node *Tree) noexcept;                            // BuildTree
		inline void Deallocate(Node *Tree) noexcept;                           // Deallocate
		inline void PrintModel(Node *Tree) const noexcept;                     // PrintModel
		inline bool IsNumber(const std::string &S) const noexcept;             // IsNumber
		inline std::size_t TreeHeight(Node *Tree) const noexcept;              // TreeHeight
		inline void LeafNodesPerLvl(Node *Tree, 
			Vec_sz_t &Lvls, int Lvl) const noexcept;                       	   // LeafNodesPerLvl
		inline float
			CalculateInfoGain(const Vec_vec_str &DataSetY,
				const Vec_vec_str &DataSetN,
				const Vec_vec_str &MainSet) const noexcept;            		   // CalculateInfoGain
		inline void 
			CalculateSetYN(Vec_vec_str &DataSetY, Vec_vec_str &DataSetN,
				const Vec_vec_str &Set,
				std::size_t Col, std::string Quest) const noexcept;    		   // CalculateSetYN

		//---Private Data--//
		const Vec_vec_str *TrainSet;
		Node              *Root;
		const Vec_str     &Attributes;

	public:
		inline DTree(const Vec_vec_str &Set,
			const Vec_str &Attrs) noexcept;                    		// Constructor
		inline ~DTree() noexcept;                                   // Destructor
		inline void BuildTree() noexcept;                           // Train-Build Model
		inline void PrintModel() const noexcept;                    // Print Model
		inline Vec_str Predict() const noexcept;                    // Answer Questions
		inline std::size_t TreeHeight() const noexcept;             // TreeHeight
};	// DTree

//-------Question Class, public Section-------//
inline DTree::Question::Question() noexcept
	: Q(""), QInfo({"", -1})
{}	// Def Constructor

inline DTree::Question::Question(const std::string &newQ, 
	const Pair_Str_Int &newQInfo) noexcept
	: Q(newQ), QInfo(newQInfo)
{}	// Constructor

inline const DTree::Question &DTree::Question::operator=(const Question &other) noexcept  
{
	if (this != &other)
	{
		this->Q = other.Q;
		this->QInfo = other.QInfo;
	}	// if
	return *this;
}	// operator = 

//-------Node Class, public Section-------//
inline DTree::Node::Node(const Vec_vec_str &newData,
	bool isLeaf, float newImpurity) noexcept
	: Leaf(isLeaf), Data(newData), Impurity(newImpurity), Children(NumOfChildren)
{}	// Constructor

inline void DTree::Node::Write() const noexcept
{
	for (const auto &i : Data)
	{
		for (const auto &j : i)
			std::cout << j << " ";
		std::cout << std::endl;
	}	// for
	std::cout << "Impurity: " << Impurity << std::endl;
	std::cout << std::endl;
}	// Write

//-------DTree Class, private Section-------//
inline void
DTree::CalculateSetYN(Vec_vec_str &DataSetY,
	Vec_vec_str &DataSetN, const Vec_vec_str &Set,
	std::size_t Col, std::string Quest) const noexcept
{
	Vec_sz_t Y;
	Vec_sz_t N;

	if (IsNumber(Quest))
	{
		std::string::size_type sz;
		double Num = std::stod(Quest, &sz);
		for (std::size_t i = 0; i < Set.size(); ++i)
			if (Num <= std::stod(Set[i][Col], &sz)) Y.push_back(i);
			else N.push_back(i);
	}	// number
	else
	{
		for (std::size_t i = 0; i < Set.size(); ++i)
			if (Quest == Set[i][Col]) Y.push_back(i);
			else N.push_back(i);
	}	// not a number

	for (std::size_t i = 0; i < Y.size(); ++i)
	{
		Vec_str Add;
		for (std::size_t j = 0; j < Set[0].size(); j++)
			Add.push_back(Set[Y[i]][j]);
		DataSetY.push_back(std::move(Add));
	}	// for

	for (std::size_t i = 0; i < N.size(); ++i)
	{
		Vec_str Add;
		for (std::size_t j = 0; j < Set[0].size(); j++)
			Add.push_back(Set[N[i]][j]);
		DataSetN.push_back(std::move(Add));
	}	// for 
}	// CalculateSetYN

inline void DTree::LeafNodesPerLvl(Node *Tree, 
	Vec_sz_t &Lvls, int Lvl) const noexcept
{
	if (Tree)
	{
		if (Tree->Leaf) Lvls[Lvl]++;
		LeafNodesPerLvl(Tree->Children[Yes], Lvls, ++Lvl);
		--Lvl;
		LeafNodesPerLvl(Tree->Children[No], Lvls, ++Lvl);
	}	// if
}	// LeafNodesPerLvl

inline std::size_t DTree::TreeHeight(Node *Tree) const noexcept
{
	if (Tree)
		return 1 + std::max(TreeHeight(Tree->Children[Yes]), 
			TreeHeight(Tree->Children[No]));
	else
		return 0;
}	// TreeHeight

inline bool DTree::IsNumber(const std::string &S) const noexcept
{
	char  *p;
	double val = strtod(S.c_str(), &p);
	return !*p;
}	// IsNumber

inline void DTree::PrintModel(Node *Tree) const noexcept
{
	if (Tree)
	{
		//if (Tree->Leaf)
			Tree->Write();
		PrintModel(Tree->Children[Yes]);
		PrintModel(Tree->Children[No]);
	}	// if
}

inline void DTree::Deallocate(Node *Tree) noexcept
{
	if (Tree)
	{
		Deallocate(Tree->Children[Yes]);
		Deallocate(Tree->Children[No]);
		delete Tree;
	}	// if
}	// Deallocate

inline void DTree::BuildTree(Node *Tree) noexcept
{
	auto Qst = FindTheRightQuestion(Tree->Data);
	if (Qst.first != "" && Qst.second != -1)
	{
		Vec_vec_str DataSetYes;
		Vec_vec_str DataSetNo;

		Tree->Qst.QInfo = Qst;
		if (IsNumber(Qst.first))
			Tree->Qst = DTree::Question("Is " + Attributes[(std::size_t)Qst.second - 1] + " >= " + Qst.first + "?", Qst);
		else
			Tree->Qst = DTree::Question("Is " + Attributes[(std::size_t)Qst.second - 1] + " " + Qst.first + "?", Qst);

		CalculateSetYN(DataSetYes, DataSetNo, Tree->Data, Qst.second, Qst.first);

		//	Trim the vectors which we we will move the mem addresses to nodes.
		DataSetYes.shrink_to_fit();
		DataSetNo.shrink_to_fit();
		for (auto &D : DataSetYes) D.shrink_to_fit();
		for (auto &D : DataSetNo) D.shrink_to_fit();

		float ImYes = CalculateImpurity(DataSetYes);
		Tree->Children[Yes] = new(std::nothrow) Node(std::move(DataSetYes), false, ImYes);
		if (!Tree->Children[Yes])
		{
			std::cerr << "Out of mem!";
			exit(1);
		}	// if
		BuildTree(Tree->Children[Yes]);

		float ImNo = CalculateImpurity(DataSetNo);
		Tree->Children[No] = new(std::nothrow) Node(std::move(DataSetNo), false, ImNo);
		if (!Tree->Children[No])
		{
			std::cerr << "Out of mem!";
			exit(1);
		}	// if
		BuildTree(Tree->Children[No]);
	}	// if
	else
	{
		Tree->Leaf = true;
		Tree->Qst = DTree::Question("No Question, Leaf Node", Qst);
	}	// else
}	// BuildTree

inline Pair_Str_Int
DTree::FindTheRightQuestion(const Vec_vec_str &Set) const noexcept
{
	// put them  in a set to remove duplicates ?
	Pair_PairStrFloat_Int Ans{ { "", 0.0f }, -1 }; 							//Pairing up the {data with impurity} with column no. 
	for (std::size_t j = 1; j < Set[0].size(); ++j)
	{
		Set_str St;
		for (std::size_t i = 0; i < Set.size(); ++i)
		{
			if (St.insert(Set[i][j]).second)								// don't check duplicates
			{
				Vec_vec_str DataSetYes;
				Vec_vec_str DataSetNo;
				CalculateSetYN(DataSetYes, DataSetNo, Set, j, Set[i][j]);
				float Ig = CalculateInfoGain(DataSetYes, DataSetNo, Set);	// Info Gain
				if (Ig > Ans.first.second)
				{
					Ans.first.first = Set[i][j];
					Ans.first.second = Ig;
					Ans.second = (int)j;
				}	// if 2
			}	// if 1
		}	// for 2
	}	// for 1
	return { Ans.first.first, Ans.second };
}	// FindTheRightQuestion

inline float 
DTree::CalculateImpurity(const Vec_vec_str &DataSet) const noexcept
{
	Map_Str_Sz_t Occ;
	float        Impurity = 1.0f;

	for (std::size_t i = 0; i < DataSet.size(); ++i) Occ[DataSet[i][Label]]++;

	for (const auto &p : Occ)
	{
		float Prob_of_label = (float)p.second / (float)DataSet.size();
		Impurity -= (float)std::pow(Prob_of_label, 2);
	}	// for

	return Impurity;
}	// CalculateImpurity

inline float
DTree::CalculateInfoGain(const Vec_vec_str &DataSetY,
	const Vec_vec_str &DataSetN,
	const Vec_vec_str &MainSet) const noexcept
{
	float P = (float)DataSetY.size() / ( (float)DataSetN.size() + (float)DataSetY.size() );
	return CalculateImpurity(MainSet) - P * CalculateImpurity(DataSetY) - ( 1 - P ) * CalculateImpurity(DataSetN); // same simpler result
	/*return ( ((float)DataSetN.size() + (float)DataSetY.size()) / TrainSet->size() ) * 
	// 	( CalculateImpurity(MainSet) - P * CalculateImpurity(DataSetY) - (1 - P) * CalculateImpurity(DataSetN) );*/
	// // (Nt / N) * (CurrImpurity - NtR / Nt * right_impurity - NtL / Nt * left_impurity)
	// // N = Whole Size, Nt = current size, NtL = yesSize, NtR = Nsize;
}	// CalculateAvgImpThroughUncertainty


//-------DTree Class, public Section-------//
inline DTree::DTree(const Vec_vec_str &Set,
	const Vec_str &Attrs) noexcept
	: TrainSet(&Set), Root(nullptr), Attributes(Attrs)
{
	if (!Set.size())	
	{
		std::cerr << "Empty set!";
		exit(1);
	}	// if

	Set_Sz_t Check;
	for (std::size_t i = 0; i < Set.size(); ++i)
		Check.insert(Set[i].size());

	if (Check.size() > 1 || *Check.begin() < 2)
	{
		std::cerr << "Wrong set, in terms of size or number of attributes!";
		exit(1);
	}	// if

	if (Attributes.size() != (*TrainSet)[0].size() - 1)
	{
		std::cerr << "Atrributes missmatch";
		exit(1);
	}	// if

	Root = new(std::nothrow) Node(Set, false, CalculateImpurity(Set));
	if (!Root)
	{
		std::cerr << "Out of memory!";
		exit(1);
	}	// if
}	// Constructor

inline DTree::~DTree() noexcept
{
	Deallocate(Root);
	Root = nullptr;
	TrainSet = nullptr;
}	// Destructor

inline void DTree::BuildTree() noexcept
{
	BuildTree(Root);
}	// Train-Build Model

inline Vec_str DTree::Predict() const noexcept
{
	Node    *Tree = Root;
	Vec_str  Ans;
	char     C = '\0';

	while (true)
	{
		if (Tree->Qst.Q == "No Question, Leaf Node")
		{
			for (std::size_t i = 0; i < Tree->Data.size(); i++)
				Ans.push_back(Tree->Data[i][Label]);
			return Ans;
		}	// if

		std::cout << Tree->Qst.Q << " (y/n)" << std::endl;
		std::cin >> C;
		if (C == 'y') Tree = Tree->Children[Yes];
		if (C == 'n') Tree = Tree->Children[No];
	}	// while

	return Ans;	// impossible
}	// Question

inline void DTree::PrintModel() const noexcept
{
	PrintModel(Root);
}	// PrintModel

inline std::size_t DTree::TreeHeight() const noexcept
{
	return TreeHeight(Root);
}	// TreeHeight