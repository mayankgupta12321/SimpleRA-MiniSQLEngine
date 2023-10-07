#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");

    if(tokenizedQuery.size() < 6 || tokenizedQuery[2] != "BY"){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }

    parsedQuery.queryType = SORT;
    
    parsedQuery.sortRelationName = tokenizedQuery[1];

    int i = 3;
    while(i < tokenizedQuery.size() && tokenizedQuery[i] != "IN") {
        parsedQuery.sortColumnList.push_back(tokenizedQuery[i]);
        i++;
    }
    i++;
    while(i < tokenizedQuery.size()) {
        if(tokenizedQuery[i] == "ASC") {
            parsedQuery.sortStrategyList.push_back(ASC);
        }
        else if(tokenizedQuery[i] == "DESC") {
            parsedQuery.sortStrategyList.push_back(DESC);
        }
        else {
            cout<<"SYNTAX ERROR"<<endl;
            return false;
        }
        i++;
    }

    if(parsedQuery.sortColumnList.size() == 0 || parsedQuery.sortStrategyList.size() == 0 || parsedQuery.sortColumnList.size() != parsedQuery.sortStrategyList.size()){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    Table table = *tableCatalogue.getTable(parsedQuery.sortRelationName);
    for (auto col : parsedQuery.sortColumnList)
    {
        if (!table.isColumn(col))
        {
            cout << "SEMANTIC ERROR: Column doesn't exist in relation";
            return false;
        }
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");
    Table table = *tableCatalogue.getTable(parsedQuery.sortRelationName);
    vector<int> columnIndices;
    for (int columnCounter = 0; columnCounter < parsedQuery.sortColumnList.size(); columnCounter++)
    {
        columnIndices.emplace_back(table.getColumnIndex(parsedQuery.sortColumnList[columnCounter]));
    }

    // Write sorting code here

    // for(int x: columnIndices) cout << x << "\n";
    // cout << "-------------------------------\n";
    
    return;
}