#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- GROUP BY grouping_column_name FROM relation_name 
 * HAVING <aggregate(attribute)> <bin_op> <attribute_value> 
 * RETURN <aggregate_func(attribute)>
 */
bool syntacticParseGROUP()
{
    logger.log("semanticParseGROUP");

    if (tokenizedQuery.size() != 13 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "HAVING" || tokenizedQuery[11] != "RETURN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = GROUP;
    parsedQuery.groupResultRelationName = tokenizedQuery[0];
    parsedQuery.groupRelationName = tokenizedQuery[6];
    parsedQuery.groupColumnName = tokenizedQuery[4];

    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.groupBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.groupBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.groupBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.groupBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.groupBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.groupBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    regex numeric("[-]?[0-9]+");
    string attribute_value = tokenizedQuery[10];
    if (regex_match(attribute_value, numeric))
    {
        parsedQuery.groupAggregateColumnValue = stoi(attribute_value);
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    string aggregate_attribute_name = tokenizedQuery[8];
    // cout << aggregate_attribute_name << " " << aggregate_attribute_name.size() << " " << aggregate_attribute_name[3] << " " << aggregate_attribute_name.back() << "\n";
    if(aggregate_attribute_name.size() <= 5 || aggregate_attribute_name[3] != '(' || aggregate_attribute_name.back() != ')') {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    string aggregate_function_name = aggregate_attribute_name.substr(0, 3);
    if (aggregate_function_name == "SUM" || aggregate_function_name == "MAX" || aggregate_function_name == "MIN" || aggregate_function_name == "AVG") {
        parsedQuery.groupAggregateFunction = aggregate_function_name;
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.groupAggregateColumnName = aggregate_attribute_name.substr(4, aggregate_attribute_name.size() - 5);

    
    string return_aggregate_attribute_name = tokenizedQuery[12];
    if(return_aggregate_attribute_name.size() <= 5 || return_aggregate_attribute_name[3] != '(' || return_aggregate_attribute_name.back() != ')') {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    string return_aggregate_function_name = return_aggregate_attribute_name.substr(0, 3);
    if (return_aggregate_function_name == "SUM" || return_aggregate_function_name == "MAX" || return_aggregate_function_name == "MIN" || return_aggregate_function_name == "AVG") {
        parsedQuery.groupReturnAggregateFunction = return_aggregate_function_name;
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.groupReturnAggregateColumnName = return_aggregate_attribute_name.substr(4, return_aggregate_attribute_name.size() - 5);

    return true;
}

bool semanticParseGROUP()
{
    logger.log("semanticParseGROUP");

    if (tableCatalogue.isTable(parsedQuery.groupResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupColumnName, parsedQuery.groupRelationName) 
        || !tableCatalogue.isColumnFromTable(parsedQuery.groupAggregateColumnName, parsedQuery.groupRelationName) 
        || !tableCatalogue.isColumnFromTable(parsedQuery.groupReturnAggregateColumnName, parsedQuery.groupRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeGROUP()
{
    logger.log("executeGROUP");

    string tempFileName = "$groupTemp_" + parsedQuery.groupRelationName;
    Table *table = tableCatalogue.getTable(parsedQuery.groupRelationName);
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

    Cursor cursor(parsedQuery.groupRelationName, 0);
    for(int i = 0; i < table->blockCount; i++) {
        vector<vector<int>> rows = cursor.getPage();
        int nRows = table->rowsPerBlockCount[i];
        bufferManager.writePage(tempFileName, i, rows, nRows);
        
        if(i + 1 < table->blockCount) cursor.nextPage(i + 1);
    }

    tableCatalogue.insertTable(tempTable);
    
    int groupColumnIndex = table->getColumnIndex(parsedQuery.groupColumnName);
    int groupAggregateColumnIndex = table->getColumnIndex(parsedQuery.groupAggregateColumnName);
    int groupReturnAggregateColumnIndex = table->getColumnIndex(parsedQuery.groupReturnAggregateColumnName);

    tempTable->sortTable({groupColumnIndex}, {ASC});

    //--------------------------------------------------

    Table *resultantTable = new Table(parsedQuery.groupResultRelationName);
    resultantTable->columns = {parsedQuery.groupColumnName,  parsedQuery.groupReturnAggregateFunction + parsedQuery.groupReturnAggregateColumnName};
    resultantTable->columnCount = resultantTable->columns.size();
    resultantTable->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * resultantTable->columnCount));
    resultantTable->indexed = table->indexed;
    resultantTable->indexedColumn = table->indexedColumn;
    resultantTable->indexingStrategy = table->indexingStrategy;

    resultantTable -> groupTable(tempTable, groupColumnIndex, parsedQuery.groupBinaryOperator, parsedQuery.groupAggregateColumnValue, parsedQuery.groupAggregateFunction, groupAggregateColumnIndex, parsedQuery.groupReturnAggregateFunction, groupReturnAggregateColumnIndex);
    
    tableCatalogue.insertTable(resultantTable);
    tableCatalogue.deleteTable(tempTable->tableName);

    return;
}