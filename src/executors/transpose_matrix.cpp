#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE MATRIX relation_name
 */

bool syntacticParseTRANSPOSE_MATRIX(){
    logger.log("syntacticParseTRANSPOSE_MATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE_MATRIX;
    parsedQuery.transposeRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseTRANSPOSE_MATRIX(){
    logger.log("semanticParseTRANSPOSE_MATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.transposeRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}   

void executeTRANSPOSE_MATRIX()
{
    logger.log("executeTRANSPOSE_MATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.transposeRelationName);
    matrix->transposeMatrix();
    return;
}