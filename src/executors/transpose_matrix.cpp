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
    
    BLOCKS_READ = 0;
    BLOCKS_WRITTEN = 0;
    
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.transposeRelationName);
    matrix->transposeMatrix();

    cout << "Number of blocks read: " << BLOCKS_READ << endl;
    cout << "Number of blocks written:: " << BLOCKS_WRITTEN << endl;
    cout << "Number of blocks accessed: " << BLOCKS_READ + BLOCKS_WRITTEN << endl;
}