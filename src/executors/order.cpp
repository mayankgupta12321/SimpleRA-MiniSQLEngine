#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- ORDER BY column_name ASC|DESC ON relation_name
 */
bool syntacticParseORDER()
{
    logger.log("syntacticParseORDER");
    if (tokenizedQuery.size() != 8 || tokenizedQuery[3] != "BY" || tokenizedQuery[6] != "ON")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = ORDER;
    parsedQuery.orderResultRelationName = tokenizedQuery[0];
    parsedQuery.orderColumnName = tokenizedQuery[4];
    parsedQuery.orderRelationName = tokenizedQuery[7];

    if(tokenizedQuery[5] == "ASC") {
        parsedQuery.orderSortingStrategy = ASC;
    }
    else if(tokenizedQuery[5] == "DESC") {
        parsedQuery.orderSortingStrategy = DESC;
    }
    else {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }

    return true;
}

bool semanticParseORDER()
{
    logger.log("semanticParseORDER");

    if (tableCatalogue.isTable(parsedQuery.orderResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.orderRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.orderColumnName, parsedQuery.orderRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeORDER()
{
    // cout << parsedQuery.orderResultRelationName << "\n";
    // cout << parsedQuery.orderRelationName << "\n";
    // cout << parsedQuery.orderColumnName << "\n";
    // cout << parsedQuery.orderSortingStrategy << "\n";
    // cout << "-------------------------------------\n";

    return;
}