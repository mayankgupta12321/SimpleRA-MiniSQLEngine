#include "global.h"
/**
 * @brief 
 * SYNTAX: RENAME MATRIX matrix_currentname matrix_newname
 */
bool syntacticParseRENAME_MATRIX()
{
    logger.log("syntacticParseRENAME_MATRIX");
    if (tokenizedQuery.size() != 4)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = RENAME_MATRIX;
    parsedQuery.renameFromMatrixName = tokenizedQuery[2];
    parsedQuery.renameToMatrixName = tokenizedQuery[3];
    return true;
}

bool semanticParseRENAME_MATRIX()
{
    logger.log("semanticParseRENAME_MATRIX");

    if (!matrixCatalogue.isMatrix(parsedQuery.renameFromMatrixName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if(matrixCatalogue.isMatrix(parsedQuery.renameToMatrixName)) {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }
    return true;
}

void executeRENAME_MATRIX()
{
    logger.log("executeRENAME_MATRIX");
    matrixCatalogue.renameMatrix(parsedQuery.renameFromMatrixName, parsedQuery.renameToMatrixName);
    return;
}