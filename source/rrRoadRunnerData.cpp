#pragma hdrstop
#include <iomanip>
#include "rrException.h"
#include "rrLogger.h"
#include "rrUtils.h"
#include "rrStringUtils.h"
#include "rrIniFile.h"
#include "rrUtils.h"
#include "Poco/TemporaryFile.h"
#include "rrRoadRunnerData.h"
#include "rrRoadRunner.h"


//---------------------------------------------------------------------------

using std::filesystem::path;

namespace rr
{

RoadRunnerData::RoadRunnerData(const int& rSize, const int& cSize ) :
        structuredResult(true),
        mTimePrecision(6),
        mDataPrecision(16)
{
    if(cSize && rSize)
    {
        allocate(rSize, cSize);
    }
}

RoadRunnerData::RoadRunnerData(const std::vector<std::string>& colNames,
        const DoubleMatrix& theData) :
        structuredResult(true),
        mTimePrecision(6),
        mDataPrecision(16),
        mColumnNames(colNames),
        mTheData(theData)
{}

RoadRunnerData::RoadRunnerData(const RoadRunner* rr) :
                structuredResult(false),
                mTimePrecision(6),
                mDataPrecision(16),
                mTheData(*rr->getSimulationData())
{
    // need const correctness here,
    // nest release, getSelections will return const vec
    RoadRunner *r = const_cast<RoadRunner*>(rr);

    const std::vector<SelectionRecord> sel = r->getSelections();

    std::vector<std::string> list(sel.size());

    for(int i = 0; i < sel.size(); ++i)
    {
        list[i] = sel[i].to_string();
    }

    setColumnNames(list);
}


RoadRunnerData::~RoadRunnerData()
{}

void RoadRunnerData::clear()
{
    mColumnNames.clear();
    mTheData.resize(0,0);
    mWeights.resize(0,0);
}

int RoadRunnerData::cSize() const
{
    return mTheData.CSize();
}

int RoadRunnerData::rSize() const
{
    return mTheData.RSize();
}

double RoadRunnerData::getTimeStart() const
{
    //Find time column
    ptrdiff_t timeCol = rr::indexOf(mColumnNames, "time");
    if(timeCol != -1)
    {
        return mTheData(0, static_cast<unsigned int>(timeCol));
    }
    return gDoubleNaN;
}

double RoadRunnerData::getTimeEnd() const
{
    //Find time column
    ptrdiff_t timeCol = rr::indexOf(mColumnNames, "time");
    if(timeCol != -1)
    {
        return mTheData(rSize() -1 ,static_cast<unsigned int>(timeCol));
    }
    return gDoubleNaN;
}

void RoadRunnerData::setName(const std::string& name)
{
    mName = name;
}

RoadRunnerData& RoadRunnerData::operator= (const RoadRunnerData& rhs)
{
    if(this == &rhs)
    {
        return *this;
    }

    mTheData = rhs.mTheData;
    mWeights = rhs.mWeights;
    mColumnNames = rhs.mColumnNames;
    return *this;
}

void RoadRunnerData::allocateWeights()
{
    //Create matrix with weights... initialize all elements to 1
    mWeights.Allocate(mTheData.RSize(), mTheData.CSize());
    for(int i = 0; i < rSize(); i++)
    {
        for(int j = 0; j < cSize(); j++)
        {
            mWeights(i,j) = 1;
        }
    }
}

bool RoadRunnerData::append(const RoadRunnerData& data)
{
    //When appending data, the number of rows have to match with current data
    if(mTheData.RSize() > 0)
    {
        if(data.rSize() != rSize())
        {
            return false;
        }
    }
    else
    {
        (*this) = data;
        return true;
    }

    int currColSize = cSize();

    RoadRunnerData temp(mColumnNames, mTheData);

    int newCSize = cSize() + data.cSize();
    mTheData.resize(data.rSize(), newCSize );

    for(int row = 0; row < temp.rSize(); row++)
    {
        for( int col = 0; col < temp.cSize(); col++)
        {
            mTheData(row, col) = temp(row, col);
        }
    }

    for(int row = 0; row < mTheData.RSize(); row++)
    {
        for(int col = 0; col < data.cSize(); col++)
        {
            mTheData(row, col + currColSize) = data(row, col);
        }
    }

    for(int col = 0; col < data.cSize(); col++)
    {
        mColumnNames.push_back(data.getColumnName(col));
    }
    return true;
}

const std::vector<std::string>& RoadRunnerData::getColumnNames() const
{
    return mColumnNames;
}

std::string RoadRunnerData::getColumnName(const int col) const
{
    if(col < mColumnNames.size())
    {
        return mColumnNames[col];
    }

    return "Bad Column..";
}

ptrdiff_t RoadRunnerData::getColumnIndex(const std::string& colName) const
{
    return rr::indexOf(mColumnNames, colName);
}

std::pair<int,int> RoadRunnerData::dimension() const
{
    return std::pair<int,int>(mTheData.RSize(), mTheData.CSize());
}

std::string RoadRunnerData::getName() const
{
    return mName;
}

void RoadRunnerData::setTimeDataPrecision(const int& prec)
{
    mTimePrecision = prec;
}

void RoadRunnerData::setDataPrecision(const int& prec)
{
    mDataPrecision = prec;
}

std::string RoadRunnerData::getColumnNamesAsString() const
{
    std::string lbls;
    for(int i = 0; i < mColumnNames.size(); i++)
    {
        lbls.append(mColumnNames[i]);
        if(i < mColumnNames.size() -1)
        {
            lbls.append(",");
        }
    }
    return lbls;
}

void RoadRunnerData::allocate(const size_t& cSize, const size_t& rSize)
{
    mTheData.Allocate(static_cast<unsigned int>(cSize), static_cast<unsigned int>(rSize));
}

//=========== OPERATORS
double& RoadRunnerData::operator() (const unsigned& row, const unsigned& col)
{
    return mTheData(row,col);
}

bool RoadRunnerData::hasWeights() const
{
    return (mWeights.size() > 0) ? true : false;
}


double RoadRunnerData::getDataElement(int row, int col)
{
    return mTheData(row,col);
}

void   RoadRunnerData::setDataElement(int row, int col, double value)
{
    mTheData(row,col) = value;
}



double RoadRunnerData::getWeight(int row, int col) const
{
    return mWeights(row, col);
}

void RoadRunnerData::setWeight(int row, int col, double value)
{
    mWeights(row, col) = value;
}

double RoadRunnerData::operator() (const unsigned& row, const unsigned& col) const
{
    return mTheData(row,col);
}

void RoadRunnerData::setColumnNames(const std::vector<std::string>& colNames)
{
    mColumnNames = colNames;
    rrLog(Logger::LOG_DEBUG) << "Simulation Data Columns: " << rr::toString(mColumnNames);
}


void RoadRunnerData::reSize(int rows, int cols)
{
    mTheData.Allocate(rows, cols);
}

void RoadRunnerData::setData(const DoubleMatrix& theData)
{
    mTheData = theData;
    rrLog(Logger::LOG_DEBUG) << "Simulation Data =========== \n" << mTheData;
    check();
}

bool RoadRunnerData::check() const
{
    if(mTheData.CSize() != mColumnNames.size())
    {
        rrLog(Logger::LOG_ERROR)<<"Number of columns ("<<mTheData.CSize()<<") in simulation data is not equal to number of columns in column header ("<<mColumnNames.size()<<")";
        return false;
    }
    return true;
}

bool RoadRunnerData::loadSimpleFormat(const std::string& fName)
{
    if(!std::filesystem::exists(fName))
    {
        return false;
    }

    std::vector<std::string> lines = getLinesInFile(fName.c_str());
    if(!lines.size())
    {
        rrLog(Logger::LOG_ERROR)<<"Failed reading/opening file "<<fName;
        return false;
    }

    mColumnNames = rr::splitString(lines[0], ",");
    rrLog(lInfo) << rr::toString(mColumnNames);

    mTheData.resize(static_cast<unsigned long>(lines.size()) -1, static_cast<unsigned long>(mColumnNames.size()));

    for(int i = 0; i < mTheData.RSize(); i++)
    {
        std::vector<std::string> aLine = splitString(lines[i+1], ", ");
        for(int j = 0; j < aLine.size(); j++)
        {
            mTheData(i,j) = toDouble(aLine[j]);
        }
    }

    return true;
}

bool RoadRunnerData::writeTo(const std::string& fileName) const
{
    std::ofstream aFile(fileName.c_str());
    if(!aFile)
    {
        rrLog(Logger::LOG_ERROR)<<"Failed opening file: "<<fileName;
        return false;
    }

    if(!check())
    {
        rrLog(Logger::LOG_ERROR)<<"Can't write data.. the dimension of the header don't agree with nr of cols of data";
        return false;
    }

    aFile<<(*this);
    aFile.close();
    return true;
}

bool RoadRunnerData::readFrom(const std::string& fileName)
{
    std::ifstream aFile(fileName.c_str());
    if(!aFile)
    {
        rrLog(Logger::LOG_ERROR)<<"Failed opening file: "<<fileName;
        return false;
    }

    aFile >> (*this);
    aFile.close();
    return true;
}

std::ostream& operator << (std::ostream& ss, const RoadRunnerData& data)
{
    //Check that the dimensions of col header and data is ok
    if (!data.check())
    {
        rrLog(Logger::LOG_ERROR) << "Can't write data. The dimension of the header don't agree with number of columns of data";
        return ss;
    }

    ss << "[INFO]" << std::endl;
    ss << "DATA_FORMAT_VERSION=1.0" << std::endl;
    ss << "CREATOR=libRoadRunner" << std::endl;
    ss << "NUMBER_OF_COLS=" << data.cSize() << std::endl;
    ss << "NUMBER_OF_ROWS=" << data.rSize() << std::endl;
    ss << "COLUMN_HEADERS=" << data.getColumnNamesAsString() << std::endl;

    ss << std::endl;
    ss << "[DATA]" << std::endl;

    RoadRunnerData::writeOnlyData(ss, data);
    RoadRunnerData::writeWeights(ss, data);

    return ss;
}

void RoadRunnerData::writeOnlyData(std::ostream& ss, const RoadRunnerData& data)
{
    //Write the headers:
    ss << data.getColumnNamesAsString() << std::endl;
    //Then the data
    for (u_int row = 0; row < data.mTheData.RSize(); row++)
    {
        for (u_int col = 0; col < data.mTheData.CSize(); col++)
        {
            if (col == 0)
            {
                ss << std::setprecision(data.mTimePrecision) << data.mTheData(row, col);
            }
            else
            {
                ss << std::setprecision(data.mDataPrecision) << data.mTheData(row, col);
            }

            if (col < data.mTheData.CSize() - 1)
            {
                ss << ",";
            }
            else
            {
                ss << std::endl;
            }
        }
    }
}

void RoadRunnerData::writeWeights(std::ostream& ss, const RoadRunnerData& data)
{
    if(data.mWeights.isAllocated())
    {
        //Write weights section
        ss<<std::endl;
        ss<<"[WEIGHTS]"<<std::endl;

        //Then the data
        for(u_int row = 0; row < data.mWeights.RSize(); row++)
        {
            for(u_int col = 0; col < data.mWeights.CSize(); col++)
            {
                if(col == 0)
                {
                    ss<<std::setprecision(data.mTimePrecision)<<data.mWeights(row, col);
                }
                else
                {
                    ss<<std::setprecision(data.mDataPrecision)<<data.mWeights(row, col);
                }

                if(col <data.mTheData.CSize() -1)
                {
                    ss << ",";
                }
                else
                {
                    ss << std::endl;
                }
            }
        }
    }
}

//Stream data from a file
std::istream& operator >> (std::istream& ss, RoadRunnerData& data)
{
    //Read in all lines into a std::string
    std::string oneLine((std::istreambuf_iterator<char>(ss)), std::istreambuf_iterator<char>());

    //This is pretty inefficient
    std::string tempFileName = (path(getTempDir()) /= "rrTempFile.dat").string();
    std::ofstream tempFile(tempFileName.c_str());

    tempFile << oneLine;
    tempFile.close();

    //Create a iniFile object and parse the data
    IniFile ini(tempFileName, true);

    IniSection* infoSection = ini.GetSection("INFO");
    if(!infoSection)
    {
        rrLog(Logger::LOG_ERROR)<<"RoadRunnder data file is missing section: [INFO]. Exiting reading file...";
        return ss;
    }

    //Setup header
    IniKey* colNames = infoSection->GetKey("COLUMN_HEADERS");

    if(!colNames)
    {
        rrLog(Logger::LOG_ERROR)<<"RoadRunnder data file is missing ini key: COLUMN_HEADERS. Exiting reading file...";
        return ss;
    }

    data.setColumnNames(splitString(colNames->mValue, ",{}")); //Remove any braces

    //Read number of cols and rows and setup data
    IniKey* aKey1 = infoSection->GetKey("NUMBER_OF_COLS");
    IniKey* aKey2 = infoSection->GetKey("NUMBER_OF_ROWS");
    if(!aKey1 || !aKey2)
    {
        rrLog(Logger::LOG_ERROR)<<"RoadRunnder data file is missing ini key: NUMBER_OF_COLS and/or NUMBER_OF_ROWS. Exiting reading file...";
        return ss;
    }

    int rDim = aKey2->AsInt();
    int cDim = aKey1->AsInt();
    data.reSize(rDim, cDim);

    //get data section
    IniSection* dataSection = ini.GetSection("DATA");
    if(!dataSection)
    {
        rrLog(Logger::LOG_ERROR)<<"RoadRunnder data file is missing ini section: DATA. Exiting reading file...";
        return ss;
    }

    std::vector<std::string> lines = splitString(dataSection->GetNonKeysAsString(), "\n");
    for(int row = 0; row < lines.size(); row++)
    {
        std::string oneLine = lines[row];
        std::vector<std::string> aLine = splitString(oneLine, ',');
        if(aLine.size() != cDim)
        {
            throw(CoreException("Bad roadrunner data in data file"));
        }

        for(int col = 0; col < cDim; col++)
        {
            rrLog(lDebug5)<<"Word "<<aLine[col];
            double value = toDouble(trim(aLine[col],' '));
            data(row, col) = value;
        }
    }

    //Weights ??

    IniSection* weightsSection = ini.GetSection("WEIGHTS");
    if(!weightsSection)    //Optional
    {
        rrLog(lDebug)<<"RoadRunnder data file is missing section: WEIGHTS. ";
        return ss;
    }
    data.mWeights.Allocate(rDim, cDim);

    lines = splitString(weightsSection->GetNonKeysAsString(), "\n");
    for(int row = 0; row < lines.size(); row++)
    {
        std::string oneLine = lines[row];
        std::vector<std::string> aLine  = splitString(oneLine, ',');
        if(aLine.size() != cDim)
        {
            throw(CoreException("Bad roadrunner data in data file"));
        }

        for(int col = 0; col < cDim; col++)
        {
            rrLog(lDebug5)<<"Word "<<aLine[col];
            double value = toDouble(aLine[col]);
            data.mWeights(row, col) = value;
        }
    }

    return ss;
}

const DoubleMatrix& RoadRunnerData::getData() const
{
    return mTheData;
}

const DoubleMatrix& RoadRunnerData::getWeights() const
{
    return mWeights;
}

}    //namespace
