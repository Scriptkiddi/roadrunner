#include <string>
#include "rrLogger.h"
#include "UnitTest++.h"
#include "rr_c_api.h"
#include "rrUtils.h"
#include "rrIniFile.h"
#include "TestUtils.h"

using namespace std;
using namespace UnitTest;

//Add using clauses..
using rr::JoinPath;
using rr::FileExists;

extern string   gTempFolder;
extern string   gDataOutputFolder;
extern string 	gTestDataFolder;
extern string   gRRInstallFolder;

extern bool 	gDebug;

//This tests is mimicking the Python tests
SUITE(CORE_EXCEPTIONS)
{
string TestDataFileName 	= "TestModel_1.dat";
IniFile iniFile;
string TestModelFileName;
RRHandle gRR;

	TEST(DATA_FILES)
	{
		gTestDataFolder 			= JoinPath(gRRInstallFolder, "tests");
		string testDataFileName 	= JoinPath(gTestDataFolder, TestDataFileName);

		CHECK(FileExists(testDataFileName));
		CHECK(iniFile.Load(testDataFileName));

		clog<<"Loaded test data from file: "<< testDataFileName;
		if(iniFile.GetSection("SBML_FILES"))
		{
			rrIniSection* sbml = iniFile.GetSection("SBML_FILES");
			rrIniKey* fNameKey = sbml->GetKey("FNAME1");
			if(fNameKey)
			{
				TestModelFileName  = JoinPath(gTestDataFolder, fNameKey->mValue);
				CHECK(FileExists(TestModelFileName));
			}
		}
        else
        {
        	CHECK(false);
        }
	}

	TEST(LOAD_SBML)
	{
		CHECK(loadSBMLFromFile(TestModelFileName.c_str()));
	}

	TEST(SET_COMPUTE_AND_ASSIGN_CONSERVATION_LAWS)
	{
		gRR = getRRInstance();
		CHECK(gRR!=NULL);
		bool res = setComputeAndAssignConservationLaws(true);
		CHECK(res);
		clog<<"\nConversation laws: "<<res<<endl;
	}

    TEST(GET_UNSCALED_ELASTICITY_COEFFICIENT)
    {

    }


}


