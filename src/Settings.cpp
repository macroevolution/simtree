#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cctype>
#include <algorithm>
#include <cstdlib>

#include "Settings.h"
#include "Log.h"
#include "MatchPathSeparator.h"


Settings::Settings(const std::string& controlFilename,
    const std::vector<UserParameter>& commandLineParameters) :
    _parameters{},
    _userParameters{},
    _commandLineParameters(commandLineParameters)
{
    readControlFile(controlFilename);

    // Get the model type
    std::string modelType;
    std::vector<UserParameter>::const_iterator it;
    for (it = _userParameters.begin(); it < _userParameters.end(); ++it) {
        if (it->first == "modeltype") {
            modelType = it->second;
            break;
        }
    }

    initializeGlobalSettings();

 
    // Re-assign parameters based on user values
    initializeSettingsWithUserValues();

    checkAllSettingsAreUserDefined();
    checkAllOutputFilesAreWriteable();
}


void Settings::readControlFile(const std::string& controlFilename)
{
    if (!fileExists(controlFilename)) {
        exitWithErrorNoControlFile();
    }

    std::ifstream controlStream(controlFilename.c_str());

    while (!controlStream.eof()) {
        std::string line;
        getline(controlStream, line, '\n');

        // Strip whitespace
        line.erase(std::remove_if(line.begin(), line.end(),
            (int(*)(int))isspace), line.end());

        // Strip comments
        std::istringstream lineStringStream(line);
        std::string lineWithoutComments;
        getline(lineStringStream, lineWithoutComments, '#');

        // Skip empty lines
        if (lineWithoutComments.size() == 0) {
            continue;
        }

        // Use second getline to split by '=' characters
        std::istringstream equalsStringStream(lineWithoutComments);
        std::vector<std::string> tokens;
        std::string token;
        while (getline(equalsStringStream, token, '=')) {
            tokens.push_back(token);
        }

        // Ensure input line is valid (two sides to an equal sign)
        if (tokens.size() != 2) {
            exitWithErrorInvalidLine(lineWithoutComments);
        }

        // Store parameter and its value
        _userParameters.push_back(UserParameter(tokens[0], tokens[1]));
    }
}


void Settings::initializeGlobalSettings()
{
    // General
    addParameter("modeltype", "bamm");
    addParameter("eventRate", "0.1");
    addParameter("lambdaInit0", "-1", NotRequired);
    addParameter("lambdaShift0", "-1", NotRequired);
    addParameter("muInit0", "-1", NotRequired);
    addParameter("maxTime", "-1");
    addParameter("minTime", "0.0", NotRequired);
    addParameter("maxNumberOfNodes", "2000", NotRequired);
    addParameter("maxTimeForEvent", "-1", NotRequired);
    addParameter("overwrite", "1", NotRequired);
    addParameter("inc", "0.1", NotRequired);
    addParameter("numberOfSims", "-1");
    addParameter("treefile", "-1");
    addParameter("eventfile", "-1");
    
    addParameter("rmin", "-1", NotRequired);
    addParameter("rmax", "-1", NotRequired);
    addParameter("rInitLogscale", "-1", NotRequired);
    addParameter("epsmin", "0", NotRequired);
    addParameter("epsmax", "1", NotRequired);
    
    addParameter("lambdaExpMean", "-1", NotRequired);
    addParameter("muExpMean", "-1", NotRequired);
    
    addParameter("newlambdashiftmax", "0.0", NotRequired);
    //addParameter("par_lambdaInit0", "-1", NotRequired);
    
    addParameter("mintaxa", "-1");
    addParameter("maxtaxa", "-1");
    
    addParameter("minNumberOfShifts", "-1");
    addParameter("maxNumberOfShifts", "-1");
 
    
/*
    addParameter("treefile", "tree.txt");
    addParameter("sampleFromPriorOnly", "0", NotRequired);
    addParameter("runMCMC", "0");
    addParameter("loadEventData", "0", NotRequired);
    addParameter("eventDataInfile", "event_data_in.txt", NotRequired);
    addParameter("initializeModel", "0");
    addParameter("simulatePriorShifts", "1", NotRequired);
    addParameter("numberOfGenerations", "0");
    addParameter("seed", "-1", NotRequired);
    addParameter("validateEventConfiguration", "1", NotRequired);
    addParameter("checkUltrametric", "1", NotRequired);

    // MCMC tuning
    addParameter("updateEventLocationScale", "0.0");
    addParameter("updateEventRateScale", "0.0");
    addParameter("localGlobalMoveRatio", "0.0");

    // Metropolis-coupled MCMC
    addParameter("numberOfChains", "1", NotRequired);
    addParameter("deltaT", "0.1", NotRequired);
    addParameter("swapPeriod", "1000", NotRequired);
    addParameter("chainSwapFileName", "chain_swap.txt", NotRequired);

    // Priors
    addParameter("poissonRatePrior", "0.0");

    // Output
    addParameter("outName", "", NotRequired);
    addParameter("runInfoFilename", "run_info.txt", NotRequired);
    addParameter("mcmcOutfile", "mcmc_out.txt", NotRequired);
    addParameter("eventDataOutfile", "event_data.txt", NotRequired);
    addParameter("priorOutputFileName", "prior_probs.txt", NotRequired);

    addParameter("branchRatesWriteFreq", "0", NotRequired);
    addParameter("mcmcWriteFreq", "0");
    addParameter("eventDataWriteFreq", "0");

    addParameter("printFreq", "0");
    addParameter("overwrite", "0", NotRequired);
    addParameter("writeMeanBranchLengthTrees", "0", NotRequired);

    addParameter("acceptanceResetFreq", "1000", NotRequired);

    // Parameter update rates
    addParameter("updateRateEventNumber", "0.0");
    addParameter("updateRateEventNumberForBranch", "0.0", NotRequired);
    addParameter("updateRateEventPosition", "0.0");
    addParameter("updateRateEventRate", "0.0");
    addParameter("initialNumberEvents", "0");

    // Other (TODO: Need to add documentation for these)
    addParameter("autotune", "0", NotRequired);
    addParameter("outputAcceptanceInfo", "0", NotRequired);
    addParameter("acceptanceInfoFileName", "acceptance_info.txt", NotRequired);

    // TODO: New params May 30 2014, need documented
    addParameter("maxNumberEvents", "500", NotRequired);
    addParameter("priorSim_IntervalGenerations", "5000", NotRequired);
    addParameter("fastSimulatePrior_Generations", "5000000", NotRequired);
    addParameter("fastSimulatePrior_SampleFreq", "50", NotRequired);
    addParameter("fastSimulatePriorExperimental", "0", NotRequired);
    addParameter("fastSimulatePrior_BurnIn", "0.05", NotRequired);

 */

    
}

void Settings::addParameter(const std::string& name, const std::string& value,
    UserDefinedStatus userDefined, DeprecationStatus deprecated)
{
    _parameters.insert(Parameter(name,
        SettingsParameter(name, value, userDefined, deprecated)));
}


void Settings::initializeSettingsWithUserValues()
{
    std::vector<std::string> paramsNotFound;

    std::vector<UserParameter>::const_iterator userParamIt;
    for (userParamIt = _userParameters.begin();
        userParamIt != _userParameters.end(); ++userParamIt) {

        // Find the matching parameter to the user-specified parameter
        ParameterMap::iterator paramIt = _parameters.find((userParamIt->first));

        // If found, set the value of the parameter to the user's
        if (paramIt != _parameters.end()) {
            // Parameter should not be deprecated
            if ((paramIt->second).isDeprecated()) {
                exitWithErrorParameterIsDeprecated(paramIt->first);
            }

            // Parameter should not already be user-defined
            if ((paramIt->second).isUserDefined()) {
                exitWithErrorDuplicateParameter(paramIt->first);
            }

            (paramIt->second).setStringValue(userParamIt->second);
        } else {
            paramsNotFound.push_back(userParamIt->first);
        }
    }

    // Handle command-line arguments
    std::vector<UserParameter>::const_iterator cmdLineParamIt;
    for (cmdLineParamIt = _commandLineParameters.begin();
        cmdLineParamIt != _commandLineParameters.end(); ++cmdLineParamIt) {
        // Find the command-line parameter in known parameter list
        ParameterMap::iterator paramIt =
            _parameters.find((cmdLineParamIt->first));

        // If found, replace current parameter value with command-line value
        if (paramIt != _parameters.end()) {
            (paramIt->second).setStringValue(cmdLineParamIt->second);
        } else {
            log(Error) << "Command-line parameter " << cmdLineParamIt->first
                << " is not a known parameter.\n";
            std::exit(1);
        }
    }

    attachPrefixToOutputFiles();

    if (paramsNotFound.size() > 0) {
        exitWithErrorParametersNotFound(paramsNotFound);
    }
}


void Settings::attachPrefixToOutputFiles()
{
    // Get the prefix string value (in outName parameter)
    ParameterMap::const_iterator it = _parameters.find("outName");
    std::string prefix;
    if (it != _parameters.end()) {
        prefix = (it->second).value<std::string>();
    }

    // Create an array of the parameters that need to be prefixed
    std::string paramsToPrefix[NumberOfParamsToPrefix] =
        { "runInfoFilename",
          "mcmcOutfile",
          "eventDataOutfile",
          "nodeStateOutfile",
          "priorOutputFileName",
          "acceptanceInfoFileName",
          "chainSwapFileName",
          "lambdaOutfile",
          "muOutfile",
          "betaOutfile" };

    // Attach the prefix to each parameter
    ParameterMap::iterator paramIt;
    for (size_t i = 0; i < NumberOfParamsToPrefix; i++) {
        paramIt = _parameters.find(paramsToPrefix[i]);
        if (paramIt != _parameters.end()) {
            const std::string& param = (paramIt->second).value<std::string>();
            (paramIt->second).setStringValue(attachPrefix(prefix, param));
        }
    }
}


std::string Settings::attachPrefix
  (const std::string& prefix, const std::string& path) const
{
    if (prefix == "") {
        return path;
    }

    const std::string& dir = extractDir(path);
    const std::string& fileName = extractFileName(path);

    return dir + prefix + "_" + fileName;
}


std::string Settings::extractDir(const std::string& path) const
{
    // Starts from the beginning, stops when it finds the last path separator
    return std::string(path.begin(), std::find_if(path.rbegin(), path.rend(),
        MatchPathSeparator()).base());
}


std::string Settings::extractFileName(const std::string& path) const
{
    // Starts from the end and stops when it finds a path separator
    return std::string(std::find_if(path.rbegin(), path.rend(),
        MatchPathSeparator()).base(), path.end());
}


void Settings::checkAllSettingsAreUserDefined() const
{
    ParameterMap::const_iterator it;
    for (it = _parameters.begin(); it != _parameters.end(); ++it) {
        const SettingsParameter& parameter = it->second;
        if (!parameter.isDeprecated()) {
            if (!parameter.isUserDefined() && parameter.mustBeUserDefined()) {
                exitWithErrorUndefinedParameter(parameter.name());
            }
        }
    }
}


void Settings::checkAllOutputFilesAreWriteable() const
{
    if (!get<bool>("overwrite")) {
        if (anyOutputFileExists()) {
            exitWithErrorOutputFileExists();
        }
    }
}


bool Settings::anyOutputFileExists() const
{
    // Global output files
    if (fileExists(get("runInfoFilename")) ||
        fileExists(get("mcmcOutfile"))     ||
        fileExists(get("eventDataOutfile"))) {
        return true;
    }

    if (get<bool>("writeMeanBranchLengthTrees")) {
        // Speciation/extinction output files
        if (get("modeltype") == "speciationextinction") {
            if (fileExists(get("lambdaOutfile")) ||
                fileExists(get("muOutfile"))) {
                return true;
            }

        // Trait output files
        } else if (get("modeltype") == "trait") {
            if (fileExists(get("betaOutfile"))) {
                return true;
            }
        }
    }

    return false;
}


bool Settings::fileExists(const std::string& filename) const
{
    std::ifstream inFile(filename.c_str());
    return inFile.good();
}


void Settings::printCurrentSettings(std::ostream& out) const
{
    int ppw = 29;

    log(Message, out) << "Current parameter settings:\n";

    ParameterMap::const_iterator it;
    for (it = _parameters.begin(); it != _parameters.end(); ++it) {
        log(Message, out) << std::right << std::setw(ppw) <<
            it->first << "    " << (it->second).value<std::string>() << "\n";
    }

    log(Message, out) << std::flush;
}


void Settings::exitWithErrorNoControlFile() const
{
    log(Error) << "Specified control file does not exist.\n"
               << "Check that the file is in the specified location.\n";
    std::exit(1);
}


void Settings::exitWithErrorInvalidLine(const std::string& line) const
{
    log(Error) << "Invalid input line in control file.\n"
               << "Problematic line includes <<" << line << ">>\n";
    std::exit(1);
}


void Settings::exitWithErrorUndefinedParameter(const std::string& name) const
{
    log(Error) << "Parameter " << name << " is undefined.\n"
               << "Fix by giving the parameter a value in the control file.\n";
    std::exit(1);
}


void Settings::exitWithErrorInvalidModelType() const
{
    log(Error) << "Invalid type of analysis.\n"
               << "Fix by setting modeltype as speciationextinction or trait\n";
    std::exit(1);
}


void Settings::exitWithErrorParametersNotFound
    (const std::vector<std::string>& paramsNotFound) const
{
    log(Error) << "One or more parameters from the control file does not\n"
        << "correspond to valid model parameters. Make sure that you are\n"
        << "running the correct version of BAMM or check that the following\n"
        << "parameters are spelled correctly:\n\n";

    std::vector<std::string>::const_iterator it;
    for (it = paramsNotFound.begin(); it != paramsNotFound.end(); ++it) {
        log() << std::setw(30) << *it << std::endl;
    }

    std::exit(1);
}

void Settings::exitWithErrorParameterIsDeprecated
    (const std::string& param) const
{
    log(Error) << "Parameter " << param << " has been deprecated. Fix by\n"
        << "removing this parameter or use the appropriate version of BAMM.\n";
    std::exit(1);
}


void Settings::exitWithErrorDuplicateParameter(const std::string& param) const
{
    log(Error) << "Duplicate parameter " << param << ".\n"
               << "Fix by removing duplicate parameter in control file.\n";
    std::exit(1);
}


void Settings::exitWithErrorOutputFileExists() const
{
    log(Error) << "Analysis is set to not overwrite files.\n"
               << "Fix by removing or renaming output file(s),\n"
               << "or set \"overwrite = 1\" in the control file.\n";
    std::exit(1);
}
