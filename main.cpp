#include "DTree.h"
#include "DataSet.h"
//defining the libraries 

inline void TrainModel(const char *FileName) noexcept;
inline void MarksRemarks(const float marks);

int main() 
{
	TrainModel("scs.csv");		// Calling the TrainModel function
	return 0;
}	// main function


/* marks out of 20 */
void MarksRemarks(const float marks){
	if(marks<=7){
		std::cout << "REMARKS: Predicted marks are less than passing criteria. Improvement required.";
	}
	if(marks>7 && marks<=13){
		std::cout << "REMARKS: Predicted met passing criteria. Improvement still required.";
	}
	if(marks>13 && marks<=18){
		std::cout << "REMARKS: Predicted score sufficient. Scope for improvement.";
	}
	if(marks>18 && marks<=20){
		std::cout << "REMARKS: Predicted score shows good performance.";
	}
} // Remarks Function

inline void TrainModel(const char *FileName) noexcept
{
	DTree  *Model;
	Vec_str	Answer;
	DataSet TrainingData(FileName);
	TrainingData.LoadSet();

	Model = new DTree(TrainingData.GetDataSet(), TrainingData.GetAtt());
	std::cout<<"Attributes in the Dataset:-"<<std::endl;
	int count=0;
	for (auto &i: TrainingData.GetAtt()){	//To print attributes used
		std::cout << i <<" ";
		count++;
	}
	std::cout<<std::endl;
	std::cout<<"Attribute size: "<<count;
	std::cout<<std::endl;

	Model->BuildTree();	// Train Model based on the set
	Answer = Model->Predict();
	std::cout << "Marks Predicted: ";
	float answer = stof(Answer[0]);
	std::cout << answer<< std::endl;
	MarksRemarks(answer);
	std::cout << std::endl << std::endl;
	Model->BuildTree();
	Model->PrintModel();
	delete Model;
}	// TrainModel function


