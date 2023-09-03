#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD MATRIX relation_name
 */

bool syntacticParseLOAD_MATRIX(){
    logger.log("syntacticParseLOAD_MATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOAD_MATRIX;
    parsedQuery.loadRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseLOAD_MATRIX(){
    logger.log("semanticParseLOAD_MATRIX");
    if (matrixCatalogue.isMatrix(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}   

void executeLOAD_MATRIX()
{
    logger.log("executeLOAD_MATRIX");
    
    BLOCKS_READ = 0;
    BLOCKS_WRITTEN = 0;

    Matrix *matrix = new Matrix(parsedQuery.loadRelationName);
    if (matrix->load())
    {
        matrixCatalogue.insertMatrix(matrix);
        cout << "Loaded Matrix => Matrix Size : " << matrix->matrixSize << "*" << matrix->matrixSize << endl;
    }

    cout << "Number of blocks read: " << BLOCKS_READ << endl;
    cout << "Number of blocks written:: " << BLOCKS_WRITTEN << endl;
    cout << "Number of blocks accessed: " << BLOCKS_READ + BLOCKS_WRITTEN << endl;
}