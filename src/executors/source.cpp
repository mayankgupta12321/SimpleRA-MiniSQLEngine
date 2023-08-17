#include "global.h"
#include <fstream>
/**
 * @brief 
 * SYNTAX: SOURCE filename
 */
bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    // cout<<"File Name: "<<parsedQuery.sourceFileName<<'\n';
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        return false;
    }
    return true;
}

void SubDoCommand()
{
    logger.log("doCommand");
    if (syntacticParse() && semanticParse())
        executeCommand();
    return;
}

void executeSOURCE()
{
    regex delim("[^\\s,]+");
    string command;
    // system("rm -rf ../data/temp");
    // system("mkdir ../data/temp");


    logger.log("executeSOURCE");
    ifstream fin;
    fin.open("../data/" + tokenizedQuery[1] + ".ra");
    while (getline(fin, command)) {
        cout<<"Command Executing : "<<command<<'\n';
        cout<<'\n';
        tokenizedQuery.clear();
        parsedQuery.clear();
        logger.log("\nReading New Command: ");
        // getline(cin, command);
        logger.log(command);


        auto words_begin = std::sregex_iterator(command.begin(), command.end(), delim);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            tokenizedQuery.emplace_back((*i).str());

        if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT")
        {
            break;
        }

        if (tokenizedQuery.empty())
        {
            continue;
        }

        if (tokenizedQuery.size() == 1)
        {
            cout << "SYNTAX ERROR" << endl;
            continue;
        }

        SubDoCommand();
        // cout << line << endl;
    }
 
    fin.close();
    return;
}
