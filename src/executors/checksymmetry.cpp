#include "global.h"
/**
 * @brief 
 * SYNTAX: CHECKSYMMETRY relation_name
 */

bool syntacticParseCHECKSYMMETRY(){
    logger.log("syntacticParseCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.checksymmetryRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCHECKSYMMETRY(){
    logger.log("semanticParseCHECKSYMMETRY");
    if (!matrixCatalogue.isMatrix(parsedQuery.checksymmetryRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}   

void executeCHECKSYMMETRY()
{
    logger.log("executeCHECKSYMMETRY");
    
    BLOCKS_READ = 0;
    BLOCKS_WRITTEN = 0;
    
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.checksymmetryRelationName);
    if(matrix->isSymmetric()) {
        cout << "TRUE" << endl;
    }
    else {
        cout << "FALSE" << endl;
    }

    cout << "Number of blocks read: " << BLOCKS_READ << endl;
    cout << "Number of blocks written:: " << BLOCKS_WRITTEN << endl;
    cout << "Number of blocks accessed: " << BLOCKS_READ + BLOCKS_WRITTEN << endl;
    
    return;
}