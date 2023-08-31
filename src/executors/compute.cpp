#include "global.h"
/**
 * @brief 
 * SYNTAX: COMPUTE relation_name
 */

bool syntacticParseCOMPUTE(){
    logger.log("syntacticParseCOMPUTE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.computeRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTE(){
    logger.log("semanticParseCOMPUTE");
    if (!matrixCatalogue.isMatrix(parsedQuery.computeRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}   

void executeCOMPUTE()
{
    logger.log("executeCOMPUTE");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.checksymmetryRelationName);
    matrix->compute();
    
    return;
}