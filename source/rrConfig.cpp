/*
 * Config.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: andy
 */

#include "rrConfig.h"
#include "rrLogger.h"
#include "rrUtils.h"

#include "tr1proxy/rr_memory.h"
#include "tr1proxy/rr_unordered_map.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <fstream> // ofstream
#include <stdexcept>

// TODO When we have gcc 4.4 as minimal compiler, drop poco and use C++ standard
// regex
#include <Poco/Path.h>
#include <Poco/RegularExpression.h>

// somebody will likely call this multithread and then bitch and moan if
// there is an issue, so lock it.
#include <thread>
#include <mutex>
// default values of sbml consistency check
#include <sbml/SBMLDocument.h>

// default values of model reset
#include "rrSelectionRecord.h"

#include <string>

using Poco::RegularExpression;
using std::string;

namespace rr {

    typedef std::unordered_map<std::string, int> StringIntMap;

    int getDefaultNumThreads() {
        int processor_count = std::thread::hardware_concurrency();
        if (processor_count == 0) // can't detect
            processor_count = 1;
        return processor_count;
    }

    /**
     * check range of key
     */
#define CHECK_RANGE(key)                                                       \
  {                                                                            \
    if (key < 0 || key >= rr::Config::CONFIG_END) {                            \
      throw std::out_of_range("invalid Config key");                           \
    }                                                                          \
  }

     /**
      * strip any leading or trailing whitespace
      */
    static std::string strip(const std::string& in) {
        std::string out;
        std::string::const_iterator b = in.begin(), e = in.end();

        // skipping leading spaces
        while (isspace(*b)) {
            ++b;
        }

        if (b != e) {
            // skipping trailing spaces
            while (isspace(*(e - 1))) {
                --e;
            }
        }

        out.assign(b, e);

        return out;
    }

    static Setting values[] = {
            Setting(false),                             // LOADSBMLOPTIONS_CONSERVED_MOIETIES
            Setting(false),                             // LOADSBMLOPTIONS_RECOMPILE
            Setting(false),                             // LOADSBMLOPTIONS_READ_ONLY
            Setting(true),                              // LOADSBMLOPTIONS_MUTABLE_INITIAL_CONDITIONS
            Setting(false),                             // LOADSBMLOPTIONS_OPTIMIZE_GVN
            Setting(false),                             // LOADSBMLOPTIONS_OPTIMIZE_CFG_SIMPLIFICATION
            Setting(false),                             // LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_COMBINING
            Setting(false),                             // LOADSBMLOPTIONS_OPTIMIZE_DEAD_INST_ELIMINATION
            Setting(false),                             // LOADSBMLOPTIONS_OPTIMIZE_DEAD_CODE_ELIMINATION
            Setting(false),                             // LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_SIMPLIFIER
            Setting(false),                             // LOADSBMLOPTIONS_USE_MCJIT
            Setting(50),                                // SIMULATEOPTIONS_STEPS,
            Setting(5.0),                               // SIMULATEOPTIONS_DURATION,
            Setting(1.e-10),                            // SIMULATEOPTIONS_ABSOLUTE,
            Setting(1.e-5),                             // SIMULATEOPTIONS_RELATIVE,
            Setting(false),                             // SIMULATEOPTIONS_STRUCTURED_RESULT,
            Setting(true),                              // SIMULATEOPTIONS_STIFF,
            Setting(false),                             // SIMULATEOPTIONS_MULTI_STEP,
            Setting(false),                             // SIMULATEOPTIONS_DETERMINISTIC_VARIABLE_STEP,
            Setting(true),                              // SIMULATEOPTIONS_STOCHASTIC_VARIABLE_STEP,
            Setting(std::string("CVODE")),          // SIMULATEOPTIONS_INTEGRATOR
            Setting(-1),                                // SIMULATEOPTIONS_INITIAL_TIMESTEP,
            Setting(-1),                                // SIMULATEOPTIONS_MINIMUM_TIMESTEP,
            Setting(-1),                                // SIMULATEOPTIONS_MAXIMUM_TIMESTEP,
            Setting(-1),                                // SIMULATEOPTIONS_MAXIMUM_NUM_STEPS
            Setting(false),                             // ROADRUNNER_DISABLE_WARNINGS
            Setting(false),                             // ROADRUNNER_DISABLE_PYTHON_DYNAMIC_PROPERTIES
            Setting(int(AllChecksON & UnitsCheckOFF)),   // SBML_APPLICABLEVALIDATORS
            Setting(0.00001),                           // ROADRUNNER_JACOBIAN_STEP_SIZE
            Setting((int)(SelectionRecord::TIME | SelectionRecord::RATE | SelectionRecord::FLOATING)), // MODEL_RESET
            Setting(1.e-12),                            // CVODE_MIN_ABSOLUTE
            Setting(1.e-6),                             // CVODE_MIN_RELATIVE
            Setting(true),                              // SIMULATEOPTIONS_COPY_RESULT
            Setting(false),                             // STEADYSTATE_PRESIMULATION
            Setting(100),                               // STEADYSTATE_PRESIMULATION_MAX_STEPS
            Setting(100.0),                             // STEADYSTATE_PRESIMULATION_TIME
            Setting(false),                             // STEADYSTATE_APPROX
            Setting(1.e-6),                             // STEADYSTATE_APPROX_TOL
            Setting(10000),                             // STEADYSTATE_APPROX_MAX_STEPS
            Setting(10000.0),                           // STEADYSTATE_APPROX_TIME
            Setting(1e-12),                             // STEADYSTATE_RELATIVE
            Setting(100),                               // STEADYSTATE_MAXIMUM_NUM_STEPS
            Setting(1e-20),                             // STEADYSTATE_MINIMUM_DAMPING
            Setting(0),                                 // STEADYSTATE_BROYDEN
            Setting(3),                                 // STEADYSTATE_LINEARITY
            Setting((std::int32_t)Config::ROADRUNNER_JACOBIAN_MODE_CONCENTRATIONS), // ROADRUNNER_JACOBIAN_MODE
            Setting(std::string(".")),              // TEMP_DIR_PATH,
            Setting(std::string("")),               // LOGGER_LOG_FILE_PATH,
            Setting(-1),                 // RANDOM_SEED
            Setting(true),                              // PYTHON_ENABLE_NAMED_MATRIX
            Setting(true),                              // LLVM_SYMBOL_CACHE
            Setting(true),                              // OPTIMIZE_REACTION_RATE_SELECTION
            Setting(true),                              // LOADSBMLOPTIONS_PERMISSIVE
            Setting(100000),                            // MAX_OUTPUT_ROWS
            Setting(false),                             // ALLOW_EVENTS_IN_STEADY_STATE_CALCULATIONS
            Setting(true),                              // VALIDATION_IN_REGENERATION
            Setting(1000),                              // K_ROWS_PER_WRITE
            Setting((std::int32_t)Config::MCJIT),       // LLVM_BACKEND
            Setting((std::int32_t)Config::NONE),        // LLJIT_OPTIMIZATION_LEVEL
            Setting(1)                      // LLJIT_NUM_THREADS
    };

    static bool initialized = false;
    static std::mutex configMutex;

    static void readDefaultConfig() {
        std::lock_guard lock(configMutex);

        if (!initialized) {
            assert(rr::Config::CONFIG_END == sizeof(values) / sizeof(Setting) &&
                "values array size different than CONFIG_END");

            std::string confPath = rr::Config::getConfigFilePath();

            try {
                if (confPath.size() > 0) {
                    rr::Config::readConfigFile(confPath);
                }
            }
            catch (std::exception& e) {
                rrLog(rr::Logger::LOG_WARNING)
                    << "error reading configuration file: " << confPath << ", "
                    << e.what();
            }
            initialized = true;
        }
    }

    /**
     * load the names of the keys and values into a std::map
     */
    static void getKeyNames(StringIntMap& keys) {
        std::lock_guard lock(configMutex);
        keys["LOADSBMLOPTIONS_CONSERVED_MOIETIES"] =
            rr::Config::LOADSBMLOPTIONS_CONSERVED_MOIETIES;
        keys["LOADSBMLOPTIONS_RECOMPILE"] = rr::Config::LOADSBMLOPTIONS_RECOMPILE;
        keys["LOADSBMLOPTIONS_READ_ONLY"] = rr::Config::LOADSBMLOPTIONS_READ_ONLY;
        keys["LOADSBMLOPTIONS_MUTABLE_INITIAL_CONDITIONS"] =
            rr::Config::LOADSBMLOPTIONS_MUTABLE_INITIAL_CONDITIONS;
        keys["LOADSBMLOPTIONS_OPTIMIZE_GVN"] =
            rr::Config::LOADSBMLOPTIONS_OPTIMIZE_GVN;
        keys["LOADSBMLOPTIONS_OPTIMIZE_CFG_SIMPLIFICATION"] =
            rr::Config::LOADSBMLOPTIONS_OPTIMIZE_CFG_SIMPLIFICATION;
        keys["LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_COMBINING"] =
            rr::Config::LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_COMBINING;
        keys["LOADSBMLOPTIONS_OPTIMIZE_DEAD_INST_ELIMINATION"] =
            rr::Config::LOADSBMLOPTIONS_OPTIMIZE_DEAD_INST_ELIMINATION;
        keys["LOADSBMLOPTIONS_OPTIMIZE_DEAD_CODE_ELIMINATION"] =
            rr::Config::LOADSBMLOPTIONS_OPTIMIZE_DEAD_CODE_ELIMINATION;
        keys["LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_SIMPLIFIER"] =
            rr::Config::LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_SIMPLIFIER;
        keys["LOADSBMLOPTIONS_USE_MCJIT"] = rr::Config::LOADSBMLOPTIONS_USE_MCJIT;
        keys["SIMULATEOPTIONS_STEPS"] = rr::Config::SIMULATEOPTIONS_STEPS;
        keys["SIMULATEOPTIONS_DURATION"] = rr::Config::SIMULATEOPTIONS_DURATION;
        keys["SIMULATEOPTIONS_ABSOLUTE"] = rr::Config::SIMULATEOPTIONS_ABSOLUTE;
        keys["SIMULATEOPTIONS_RELATIVE"] = rr::Config::SIMULATEOPTIONS_RELATIVE;
        keys["SIMULATEOPTIONS_STRUCTURED_RESULT"] =
            rr::Config::SIMULATEOPTIONS_STRUCTURED_RESULT;
        keys["SIMULATEOPTIONS_STIFF"] = rr::Config::SIMULATEOPTIONS_STIFF;
        keys["SIMULATEOPTIONS_MULTI_STEP"] = rr::Config::SIMULATEOPTIONS_MULTI_STEP;
        keys["SIMULATEOPTIONS_DETERMINISTIC_VARIABLE_STEP"] =
            rr::Config::SIMULATEOPTIONS_DETERMINISTIC_VARIABLE_STEP;
        keys["SIMULATEOPTIONS_STOCHASTIC_VARIABLE_STEP"] =
            rr::Config::SIMULATEOPTIONS_STOCHASTIC_VARIABLE_STEP;
        keys["SIMULATEOPTIONS_INTEGRATOR"] = rr::Config::SIMULATEOPTIONS_INTEGRATOR;
        keys["SIMULATEOPTIONS_INITIAL_TIMESTEP"] =
            rr::Config::SIMULATEOPTIONS_INITIAL_TIMESTEP;
        keys["SIMULATEOPTIONS_MINIMUM_TIMESTEP"] =
            rr::Config::SIMULATEOPTIONS_MINIMUM_TIMESTEP;
        keys["SIMULATEOPTIONS_MAXIMUM_TIMESTEP"] =
            rr::Config::SIMULATEOPTIONS_MAXIMUM_TIMESTEP;
        keys["SIMULATEOPTIONS_MAXIMUM_NUM_STEPS"] =
            rr::Config::SIMULATEOPTIONS_MAXIMUM_NUM_STEPS;
        keys["ROADRUNNER_DISABLE_WARNINGS"] = rr::Config::ROADRUNNER_DISABLE_WARNINGS;
        keys["ROADRUNNER_DISABLE_PYTHON_DYNAMIC_PROPERTIES"] =
            rr::Config::ROADRUNNER_DISABLE_PYTHON_DYNAMIC_PROPERTIES;
        keys["SBML_APPLICABLEVALIDATORS"] = rr::Config::SBML_APPLICABLEVALIDATORS;
        keys["ROADRUNNER_JACOBIAN_STEP_SIZE"] =
            rr::Config::ROADRUNNER_JACOBIAN_STEP_SIZE;
        keys["MODEL_RESET"] = rr::Config::MODEL_RESET;
        keys["CVODE_MIN_ABSOLUTE"] = rr::Config::CVODE_MIN_ABSOLUTE;
        keys["CVODE_MIN_RELATIVE"] = rr::Config::CVODE_MIN_RELATIVE;
        keys["SIMULATEOPTIONS_COPY_RESULT"] = rr::Config::SIMULATEOPTIONS_COPY_RESULT;
        keys["STEADYSTATE_PRESIMULATION"] = rr::Config::STEADYSTATE_PRESIMULATION;
        keys["STEADYSTATE_PRESIMULATION_MAX_STEPS"] =
            rr::Config::STEADYSTATE_PRESIMULATION_MAX_STEPS;
        keys["STEADYSTATE_PRESIMULATION_TIME"] =
            rr::Config::STEADYSTATE_PRESIMULATION_TIME;
        keys["STEADYSTATE_APPROX"] = rr::Config::STEADYSTATE_APPROX;
        keys["STEADYSTATE_APPROX_TOL"] = rr::Config::STEADYSTATE_APPROX_TOL;
        keys["STEADYSTATE_APPROX_MAX_STEPS"] =
            rr::Config::STEADYSTATE_APPROX_MAX_STEPS;
        keys["STEADYSTATE_APPROX_TIME"] = rr::Config::STEADYSTATE_APPROX_TIME;
        keys["STEADYSTATE_RELATIVE"] = rr::Config::STEADYSTATE_RELATIVE;
        keys["STEADYSTATE_MAXIMUM_NUM_STEPS"] =
            rr::Config::STEADYSTATE_MAXIMUM_NUM_STEPS;
        keys["STEADYSTATE_MINIMUM_DAMPING"] = rr::Config::STEADYSTATE_MINIMUM_DAMPING;
        keys["STEADYSTATE_BROYDEN"] = rr::Config::STEADYSTATE_BROYDEN;
        keys["STEADYSTATE_LINEARITY"] = rr::Config::STEADYSTATE_LINEARITY;
        keys["ROADRUNNER_JACOBIAN_MODE"] = rr::Config::ROADRUNNER_JACOBIAN_MODE;
        keys["TEMP_DIR_PATH"] = rr::Config::TEMP_DIR_PATH;
        keys["LOGGER_LOG_FILE_PATH"] = rr::Config::LOGGER_LOG_FILE_PATH;
        keys["RANDOM_SEED"] = rr::Config::RANDOM_SEED;
        keys["PYTHON_ENABLE_NAMED_MATRIX"] = rr::Config::PYTHON_ENABLE_NAMED_MATRIX;
        keys["LLVM_SYMBOL_CACHE"] = rr::Config::LLVM_SYMBOL_CACHE;
        keys["OPTIMIZE_REACTION_RATE_SELECTION"] =
            rr::Config::OPTIMIZE_REACTION_RATE_SELECTION;
        keys["LOADSBMLOPTIONS_PERMISSIVE"] = rr::Config::LOADSBMLOPTIONS_PERMISSIVE;
        keys["MAX_OUTPUT_ROWS"] = rr::Config::MAX_OUTPUT_ROWS;
        keys["ALLOW_EVENTS_IN_STEADY_STATE_CALCULATIONS"] =
            rr::Config::ALLOW_EVENTS_IN_STEADY_STATE_CALCULATIONS;
        keys["VALIDATION_IN_REGENERATION"] = rr::Config::VALIDATION_IN_REGENERATION;
        keys["K_ROWS_PER_WRITE"] = rr::Config::K_ROWS_PER_WRITE;
        keys["LLVM_BACKEND"] = rr::Config::LLVM_BACKEND_VALUES::MCJIT;
        keys["LLJIT_OPTIMIZATION_LEVEL"] = rr::Config::LLJIT_OPTIMIZATION_LEVELS::AGGRESSIVE;
        keys["LLJIT_NUM_THREADS"] = getDefaultNumThreads();

        // add space after develop keys to clean up merging.

        assert(rr::Config::CONFIG_END == sizeof(values) / sizeof(Setting) &&
            "values array size different than CONFIG_END");
        assert(rr::Config::CONFIG_END == keys.size() &&
            "number of keys in std::map does not match static values");
    }

    static std::string reverseLookup(StringIntMap& keys, Config::Keys k) {
        std::lock_guard lock(configMutex);
        for (StringIntMap::iterator i = keys.begin(); i != keys.end(); ++i) {
            if (i->second == k)
                return i->first;
        }
        throw std::runtime_error("No such key");
    }

    std::vector<std::string> Config::getKeyList() {
        std::vector<std::string> result;
        StringIntMap m;

        getKeyNames(m);

        for (int n = 0; n < CONFIG_END; ++n) {
            try {
                std::string key_str = reverseLookup(m, (Config::Keys)n);
                result.push_back(key_str);
            }
            catch (std::runtime_error) {
                continue;
            }
        }

        return result;
    }

    std::string Config::getString(Keys key) {
        readDefaultConfig();
        CHECK_RANGE(key);
        return values[key].toString();
    }

    int Config::getInt(Keys key) {
        readDefaultConfig();
        CHECK_RANGE(key);
        return values[key].get<int>();
    }

    double Config::getDouble(Keys key) {
        readDefaultConfig();
        CHECK_RANGE(key);
        return values[key].get<double>();
    }

    std::string Config::getConfigFilePath() {
        // check env var
        const char* env = std::getenv("ROADRUNNER_CONFIG");
        std::string path;
        Poco::Path ppath;

        rrLog(rr::Logger::LOG_DEBUG)
            << "trying config file from ROADRUNNER_CONFIG " << (env ? env : "NULL");

        if (env && std::filesystem::exists(env)) {
            return env;
        }

        // check home dir
        ppath.assign(Poco::Path::home());
        ppath.setFileName("roadrunner.conf");
        path = ppath.toString();
        rrLog(rr::Logger::LOG_DEBUG) << "trying config file " << path;
        if (std::filesystem::exists(path)) {
            return path;
        }

        ppath.setFileName(".roadrunner.conf");
        path = ppath.toString();
        rrLog(rr::Logger::LOG_DEBUG) << "trying config file " << path;
        if (std::filesystem::exists(path)) {
            return path;
        }

        // this could be an empty std::string if we are in a statically
        // linked executable, if so, Poco::Path will puke if popDir is called
        std::string chkDir = rr::getCurrentSharedLibDir();
        if (chkDir.empty()) {
            chkDir = rr::getCurrentExeFolder();
        }

        assert(!chkDir.empty() && "could not get either shared lib or exe dir");

        // check in library dir
        ppath.assign(chkDir);
        ppath.setFileName("roadrunner.conf");
        path = ppath.toString();
        rrLog(rr::Logger::LOG_DEBUG) << "trying config file " << path;
        if (std::filesystem::exists(path)) {
            return path;
        }

        // check one level up
        ppath.assign(chkDir);
        ppath.popDirectory();
        ppath.setFileName("roadrunner.conf");
        path = ppath.toString();
        rrLog(rr::Logger::LOG_DEBUG) << "trying config file " << path;
        if (std::filesystem::exists(path)) {
            return path;
        }

        rrLog(rr::Logger::LOG_DEBUG) << "no config file found; using built-in defaults";
        return "";
    }

    void Config::setValue(Keys key, Setting value) {
        readDefaultConfig();
        CHECK_RANGE(key);
        values[key] = std::move(value);
    }

    /*void Config::setValues(const std::vector<Keys> keys, const
        std::vector<Setting> values)
    {
        auto keyit = keys.begin();
        auto valueit = values.begin();
        while (keyit != keys.end() && valueit != values.end()) {
            setValue(*keyit, *valueit);
        }
    }*/

    void Config::readConfigFile(const std::string& path) {

        const Poco::RegularExpression re(R"(^\s*(\w*)\s*:\s*(.*)\s*$)",
            RegularExpression::RE_CASELESS);
        StringIntMap keys;
        std::ifstream in(path.c_str());

        if (!in) {
            throw std::ifstream::failure("could not open " + path + " for reading");
        }

        getKeyNames(keys);

        std::string line;

        while (std::getline(in, line)) {
            std::vector<std::string> matches;

            int nmatch = re.split(line, matches);

            if (nmatch == 3) {
                StringIntMap::const_iterator i = keys.find(matches[1]);
                if (i != keys.end()) {
                    values[i->second] = Setting::parse((matches[2]));
                    rrLog(Logger::LOG_INFORMATION)
                        << "read key " << i->first
                        << " with value: " << values[i->second].get<std::string>();
                }
                else {
                    rrLog(Logger::LOG_WARNING)
                        << "invalid key: \"" << matches[1] << "\" in " << path;
                }
            }
        }

        initialized = true;
    }

    Setting Config::getValue(Keys key) {
        readDefaultConfig();
        CHECK_RANGE(key);
        return values[key];
    }

    bool Config::getBool(Keys key) {
        readDefaultConfig();
        CHECK_RANGE(key);
        return values[key].get<bool>();
    }

    void Config::writeConfigFile(const std::string& path) {
        std::ofstream out(path.c_str());

        if (!out) {
            throw std::ofstream::failure("could not open " + path + " for writing");
        }

        StringIntMap keys;
        std::ifstream in(path.c_str());

        getKeyNames(keys);

        for (auto [keyName, keyEnumeration] : keys) {
            out << keyName << ": " << values[keyEnumeration].toString() << std::endl;
        }
    }

    Config::Keys Config::stringToKey(const std::string& key) {
        if (key == "LOADSBMLOPTIONS_CONSERVED_MOIETIES")
            return Config::LOADSBMLOPTIONS_CONSERVED_MOIETIES;
        else if (key == "LOADSBMLOPTIONS_RECOMPILE")
            return Config::LOADSBMLOPTIONS_RECOMPILE;
        else if (key == "LOADSBMLOPTIONS_READ_ONLY")
            return Config::LOADSBMLOPTIONS_READ_ONLY;
        else if (key == "LOADSBMLOPTIONS_MUTABLE_INITIAL_CONDITIONS")
            return Config::LOADSBMLOPTIONS_MUTABLE_INITIAL_CONDITIONS;
        else if (key == "LOADSBMLOPTIONS_OPTIMIZE_GVN")
            return Config::LOADSBMLOPTIONS_OPTIMIZE_GVN;
        else if (key == "LOADSBMLOPTIONS_OPTIMIZE_CFG_SIMPLIFICATION")
            return Config::LOADSBMLOPTIONS_OPTIMIZE_CFG_SIMPLIFICATION;
        else if (key == "LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_COMBINING")
            return Config::LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_COMBINING;
        else if (key == "LOADSBMLOPTIONS_OPTIMIZE_DEAD_INST_ELIMINATION")
            return Config::LOADSBMLOPTIONS_OPTIMIZE_DEAD_INST_ELIMINATION;
        else if (key == "LOADSBMLOPTIONS_OPTIMIZE_DEAD_CODE_ELIMINATION")
            return Config::LOADSBMLOPTIONS_OPTIMIZE_DEAD_CODE_ELIMINATION;
        else if (key == "LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_SIMPLIFIER")
            return Config::LOADSBMLOPTIONS_OPTIMIZE_INSTRUCTION_SIMPLIFIER;
        else if (key == "LOADSBMLOPTIONS_USE_MCJIT")
            return Config::LOADSBMLOPTIONS_USE_MCJIT;
        else if (key == "SIMULATEOPTIONS_STEPS")
            return Config::SIMULATEOPTIONS_STEPS;
        else if (key == "SIMULATEOPTIONS_DURATION")
            return Config::SIMULATEOPTIONS_DURATION;
        else if (key == "SIMULATEOPTIONS_ABSOLUTE")
            return Config::SIMULATEOPTIONS_ABSOLUTE;
        else if (key == "SIMULATEOPTIONS_RELATIVE")
            return Config::SIMULATEOPTIONS_RELATIVE;
        else if (key == "SIMULATEOPTIONS_STRUCTURED_RESULT")
            return Config::SIMULATEOPTIONS_STRUCTURED_RESULT;
        else if (key == "SIMULATEOPTIONS_STIFF")
            return Config::SIMULATEOPTIONS_STIFF;
        else if (key == "SIMULATEOPTIONS_MULTI_STEP")
            return Config::SIMULATEOPTIONS_MULTI_STEP;
        else if (key == "SIMULATEOPTIONS_DETERMINISTIC_VARIABLE_STEP")
            return Config::SIMULATEOPTIONS_DETERMINISTIC_VARIABLE_STEP;
        else if (key == "SIMULATEOPTIONS_STOCHASTIC_VARIABLE_STEP")
            return Config::SIMULATEOPTIONS_STOCHASTIC_VARIABLE_STEP;
        else if (key == "SIMULATEOPTIONS_INTEGRATOR")
            return Config::SIMULATEOPTIONS_INTEGRATOR;
        else if (key == "SIMULATEOPTIONS_INITIAL_TIMESTEP")
            return Config::SIMULATEOPTIONS_INITIAL_TIMESTEP;
        else if (key == "SIMULATEOPTIONS_MINIMUM_TIMESTEP")
            return Config::SIMULATEOPTIONS_MINIMUM_TIMESTEP;
        else if (key == "SIMULATEOPTIONS_MAXIMUM_TIMESTEP")
            return Config::SIMULATEOPTIONS_MAXIMUM_TIMESTEP;
        else if (key == "SIMULATEOPTIONS_MAXIMUM_NUM_STEPS")
            return Config::SIMULATEOPTIONS_MAXIMUM_NUM_STEPS;
        else if (key == "ROADRUNNER_DISABLE_WARNINGS")
            return Config::ROADRUNNER_DISABLE_WARNINGS;
        else if (key == "ROADRUNNER_DISABLE_PYTHON_DYNAMIC_PROPERTIES")
            return Config::ROADRUNNER_DISABLE_PYTHON_DYNAMIC_PROPERTIES;
        else if (key == "SBML_APPLICABLEVALIDATORS")
            return Config::SBML_APPLICABLEVALIDATORS;
        else if (key == "ROADRUNNER_JACOBIAN_STEP_SIZE")
            return Config::ROADRUNNER_JACOBIAN_STEP_SIZE;
        else if (key == "MODEL_RESET")
            return Config::MODEL_RESET;
        else if (key == "CVODE_MIN_ABSOLUTE")
            return Config::CVODE_MIN_ABSOLUTE;
        else if (key == "CVODE_MIN_RELATIVE")
            return Config::CVODE_MIN_RELATIVE;
        else if (key == "SIMULATEOPTIONS_COPY_RESULT")
            return Config::SIMULATEOPTIONS_COPY_RESULT;
        else if (key == "STEADYSTATE_PRESIMULATION")
            return Config::STEADYSTATE_PRESIMULATION;
        else if (key == "STEADYSTATE_PRESIMULATION_MAX_STEPS")
            return Config::STEADYSTATE_PRESIMULATION_MAX_STEPS;
        else if (key == "STEADYSTATE_PRESIMULATION_TIME")
            return Config::STEADYSTATE_PRESIMULATION_TIME;
        else if (key == "STEADYSTATE_APPROX")
            return Config::STEADYSTATE_APPROX;
        else if (key == "STEADYSTATE_APPROX_TOL")
            return Config::STEADYSTATE_APPROX_TOL;
        else if (key == "STEADYSTATE_APPROX_MAX_STEPS")
            return Config::STEADYSTATE_APPROX_MAX_STEPS;
        else if (key == "STEADYSTATE_APPROX_TIME")
            return Config::STEADYSTATE_APPROX_TIME;
        else if (key == "STEADYSTATE_RELATIVE")
            return Config::STEADYSTATE_RELATIVE;
        else if (key == "STEADYSTATE_MAXIMUM_NUM_STEPS")
            return Config::STEADYSTATE_MAXIMUM_NUM_STEPS;
        else if (key == "STEADYSTATE_MINIMUM_DAMPING")
            return Config::STEADYSTATE_MINIMUM_DAMPING;
        else if (key == "STEADYSTATE_BROYDEN")
            return Config::STEADYSTATE_BROYDEN;
        else if (key == "STEADYSTATE_LINEARITY")
            return Config::STEADYSTATE_LINEARITY;
        else if (key == "ROADRUNNER_JACOBIAN_MODE")
            return Config::ROADRUNNER_JACOBIAN_MODE;
        else if (key == "TEMP_DIR_PATH")
            return Config::TEMP_DIR_PATH;
        else if (key == "LOGGER_LOG_FILE_PATH")
            return Config::LOGGER_LOG_FILE_PATH;
        else if (key == "RANDOM_SEED")
            return Config::RANDOM_SEED;
        else if (key == "PYTHON_ENABLE_NAMED_MATRIX")
            return Config::PYTHON_ENABLE_NAMED_MATRIX;
        else if (key == "LLVM_SYMBOL_CACHE")
            return Config::LLVM_SYMBOL_CACHE;
        else if (key == "OPTIMIZE_REACTION_RATE_SELECTION")
            return Config::OPTIMIZE_REACTION_RATE_SELECTION;
        else if (key == "LOADSBMLOPTIONS_PERMISSIVE")
            return Config::LOADSBMLOPTIONS_PERMISSIVE;
        else if (key == "MAX_OUTPUT_ROWS")
            return Config::MAX_OUTPUT_ROWS;
        else if (key == "ALLOW_EVENTS_IN_STEADY_STATE_CALCULATIONS")
            return Config::ALLOW_EVENTS_IN_STEADY_STATE_CALCULATIONS;
        else if (key == "VALIDATION_IN_REGENERATION")
            return Config::VALIDATION_IN_REGENERATION;
        else if (key == "LLVM_BACKEND")
            return Config::LLVM_BACKEND;
        else if (key == "LLJIT_OPTIMIZATION_LEVEL")
            return Config::LLJIT_OPTIMIZATION_LEVEL;
        else if (key == "LLJIT_NUM_THREADS")
            return Config::LLJIT_NUM_THREADS;

        else
            throw std::runtime_error("No such config key: '" + key + "'");
    }

} // namespace rr