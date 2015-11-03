#include "QuizModule.h"
#include <stdio.h>
#include <fstream>
#include <sstream>


QuizModule::QuizModule()
{
	mModuleName = "Quiz Module";
}

QuizModule::~QuizModule()
{
	if (!mHasShutDown)
		Shutdown();
}
// TODO : Return available commands that we should listen to
bool QuizModule::Init()
{
	// Open up file ModuleData\Questions\question_sets.txt
	// parse each line as an available question set.
	// scan in
	mCurrentQuestion = 0;
	mWorkingDirectory += "Questions\\";
	mHasInitializedSuccessfully = CheckForQuestionSets("question_sets.txt");
	
	printf("Quizbot Activated\n");
	return mHasInitializedSuccessfully;
}

void QuizModule::SetModuleName()
{
	mModuleName = "QuizBot";
}

void QuizModule::Shutdown()
{
	mHasShutDown = true;
	printf("Shutting down Quizbot...\n");
}

bool QuizModule::Start(const std::string& questionset, const std::string& userType, const int& roundTime)
{
	bool success = false;

	if (!mHasInitializedSuccessfully)
		success = Init();

	if (userType == "mod" || userType == "admin")
	{
		success = ReadQuestionFile(questionset);
		mRoundTime = roundTime;
	}

	return success;
}

void QuizModule::ParseAnswer(const std::string& answerData)
{
	printf("Checking to see if this was a correct answer\n");
}

bool QuizModule::ReadQuestionFile(const std::string& questionSetName)
{
	std::string toCheck = questionSetName;
	std::stringstream checkExtension;
	std::string line;
	bool extensionCheck = false;
	bool questionSetFound = false;
	QuestionData* dataToFill = 0;

	// Just perform a sanity check to see if the user has supplied a name that ends in .txt since we store that in the question set data.
	int length = questionSetName.length();

	// check to see the end of the suppled string is ".txt"
	for (int i = 4; i > 0; i--)
	{
		checkExtension << questionSetName[length - i];
	}

	if (checkExtension.str() != ".txt")
		toCheck.append(".txt");


	// We'll then need to check to see if any of the question sets contains this name.
	for (size_t i = 0; i < mAvailableQuestionSets.size(); i++)
	{
		if (mAvailableQuestionSets.at(i)->QuestionSetName == toCheck)
		{
			dataToFill = mAvailableQuestionSets.at(i);
			break;
		}
	}

	if (dataToFill != 0)
	{
		std::stringstream fullPath;
		fullPath << mWorkingDirectory << dataToFill->QuestionSetName;
		std::ifstream infile(fullPath.str());

		// Open the file
		if (!infile.fail())
		{
			while (getline(infile, line))
			{
				// Parse this file, open it up and check to see if it has any questions in it, otherwise there is no point in adding it.
				// Read and copy a line into the collection set.
				// TODO : check to see if this contains valid data.
				std::stringstream questionStream;
				std::stringstream answerStream;
				QuestionInfo newInfo;
				bool splitRead = false;
				for (size_t i = 0; i < line.length(); ++i)
				{
					if (line[i] == ':')
					{
						splitRead = true;
						++i;
					}

					if (splitRead == false)
						questionStream << line[i];
					else
						answerStream << line[i];
				}

				newInfo.Question = questionStream.str();
				newInfo.Answer = answerStream.str();
				dataToFill->Data.push_back(newInfo);
			}
		}

		return true;
	}

	return false;
}

bool QuizModule::CheckForQuestionSets(const std::string& filenameoflist)
{
	std::stringstream fullPath;
	std::string line;
	bool someData = false;
	
	fullPath << mWorkingDirectory << filenameoflist;
	
	std::ifstream infile(fullPath.str());

	// Open the file
	if (!infile.fail())
	{
		while (getline(infile, line))
		{
			// Parse this file, open it up and check to see if it has any questions in it, otherwise there is no point in adding it.
			// Read and copy a line into the collection set.
			// TODO : check to see if this contains valid data.
			QuestionData* pData = new QuestionData;
			pData->QuestionSetName = line;
			mAvailableQuestionSets.push_back(pData);
			someData = true;
		}
	}
	
	return someData;
}
