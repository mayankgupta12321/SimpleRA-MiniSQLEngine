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
    logger.log("executeORDER");
    
    Table *table = tableCatalogue.getTable(parsedQuery.orderRelationName);
    Table *resultantTable = new Table(parsedQuery.orderResultRelationName);

    resultantTable->columns = table->columns;
    resultantTable->distinctValuesPerColumnCount = table->distinctValuesPerColumnCount;
    resultantTable->columnCount = table->columnCount;
    resultantTable->rowCount = table->rowCount;
    resultantTable->blockCount = table->blockCount;
    resultantTable->maxRowsPerBlock = table->maxRowsPerBlock;
    resultantTable->rowsPerBlockCount = table->rowsPerBlockCount;
    resultantTable->indexed = table->indexed;
    resultantTable->indexedColumn = table->indexedColumn;
    resultantTable->indexingStrategy = table->indexingStrategy;

    
    Cursor cursor(parsedQuery.orderRelationName, 0);
    for(int i = 0; i < table->blockCount; i++) {
        vector<vector<int>> rows = cursor.getPage();
        int nRows = table->rowsPerBlockCount[i];
        bufferManager.writePage(parsedQuery.orderResultRelationName, i, rows, nRows);
        
        if(i + 1 < table->blockCount) cursor.nextPage(i + 1);
    }

    tableCatalogue.insertTable(resultantTable);

    int columnIndices = table->getColumnIndex(parsedQuery.orderColumnName);
    resultantTable->sortTable({columnIndices}, {parsedQuery.orderSortingStrategy});
    return;
}