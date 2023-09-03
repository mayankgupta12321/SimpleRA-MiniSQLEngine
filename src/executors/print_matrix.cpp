#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT MATRIX relation_name
 */
bool syntacticParsePRINT_MATRIX()
{
    logger.log("syntacticParsePRINT_MATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT_MATRIX;
    parsedQuery.printRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINT_MATRIX()
{
    logger.log("semanticParsePRINT_MATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINT_MATRIX()
{
    logger.log("executePRINT_MATRIX");
    
    BLOCKS_READ = 0;
    BLOCKS_WRITTEN = 0;

    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
    matrix->print();

    cout << "Number of blocks read: " << BLOCKS_READ << endl;
    cout << "Number of blocks written:: " << BLOCKS_WRITTEN << endl;
    cout << "Number of blocks accessed: " << BLOCKS_READ + BLOCKS_WRITTEN << endl;
}
