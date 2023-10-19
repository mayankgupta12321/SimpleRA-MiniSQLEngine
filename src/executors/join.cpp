#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

Table* createDeepCopyOfTable(string joinRelationName, string joinColumnName) {

    string tempFileName = "$joinTemp_" + joinRelationName;
    Table *table = tableCatalogue.getTable(joinRelationName);
    Table *tempTable = new Table(tempFileName);

    tempTable->columns = table->columns;
    tempTable->distinctValuesPerColumnCount = table->distinctValuesPerColumnCount;
    tempTable->columnCount = table->columnCount;
    tempTable->rowCount = table->rowCount;
    tempTable->blockCount = table->blockCount;
    tempTable->maxRowsPerBlock = table->maxRowsPerBlock;
    tempTable->rowsPerBlockCount = table->rowsPerBlockCount;
    tempTable->indexed = table->indexed;
    tempTable->indexedColumn = table->indexedColumn;
    tempTable->indexingStrategy = table->indexingStrategy;

    Cursor cursor(joinRelationName, 0);
    for(int i = 0; i < table->blockCount; i++) {
        vector<vector<int>> rows = cursor.getPage();
        int nRows = table->rowsPerBlockCount[i];
        bufferManager.writePage(tempFileName, i, rows, nRows);
        
        if(i + 1 < table->blockCount) cursor.nextPage(i + 1);
    }

    tableCatalogue.insertTable(tempTable);
    int joinColumnIndex = table->getColumnIndex(joinColumnName);
    tempTable->sortTable({joinColumnIndex}, {ASC});

    return tempTable;
}
void executeJOIN()
{
    logger.log("executeJOIN");

    Table *tempTable1 = createDeepCopyOfTable(parsedQuery.joinFirstRelationName, parsedQuery.joinFirstColumnName);
    Table *tempTable2 = createDeepCopyOfTable(parsedQuery.joinSecondRelationName, parsedQuery.joinSecondColumnName);

    int joinFirstColumnIndex = tempTable1->getColumnIndex(parsedQuery.joinFirstColumnName);
    int joinSecondColumnIndex = tempTable2->getColumnIndex(parsedQuery.joinSecondColumnName);

    // Resultant Table
    Table *resultantTable = new Table(parsedQuery.joinResultRelationName);

    resultantTable->columns = {};
    for(string columnName : tempTable1->columns) {
        resultantTable->columns.push_back(columnName);
    }

    for(string columnName : tempTable2->columns) {
        resultantTable->columns.push_back(columnName);
    }

    resultantTable->columnCount = resultantTable->columns.size();
    resultantTable->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * resultantTable->columnCount));

    resultantTable -> joinTable(tempTable1, tempTable2, joinFirstColumnIndex, joinSecondColumnIndex, parsedQuery.joinBinaryOperator);

    tableCatalogue.insertTable(resultantTable);
    tableCatalogue.deleteTable(tempTable1->tableName);
    tableCatalogue.deleteTable(tempTable2->tableName);
}