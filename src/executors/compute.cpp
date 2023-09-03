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

vector<vector<int>> transpose(vector<vector<int>> &grid) {
    int n = grid.size();
    int m = grid[0].size();

    vector<vector<int>> grid_t(m ,vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            grid_t[j][i] = grid[i][j];
        }
    }

    return grid_t;
}

vector<vector<int>> compute(vector<vector<int>> grid, vector<vector<int>> grid_T) {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            grid[i][j] -= grid_T[i][j];
        }
    }
    return grid;
}

void executeCOMPUTE()
{
    logger.log("executeCOMPUTE");
    
    BLOCKS_READ = 0;
    BLOCKS_WRITTEN = 0;
    
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.computeRelationName);
    string matrixName = matrix->matrixName;
    unsigned int matrixSize = matrix->matrixSize;
    unsigned int smallMatrixSize = matrix->smallMatrixSize;
    unsigned int smallMatrixCount = matrix->smallMatrixCount;
    unsigned int blockCount = matrix->blockCount;

    string resultantMatrixName = matrixName + "_RESULT";
    Matrix *resultantMatrix = new Matrix(resultantMatrixName);
    resultantMatrix->matrixSize = matrixSize;
    resultantMatrix->smallMatrixSize = smallMatrixSize;
    resultantMatrix->smallMatrixCount = smallMatrixCount;
    resultantMatrix->blockCount = blockCount;
    
    for(int pageRowCounter = 0; pageRowCounter < smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = pageRowCounter; pageColumnCounter < smallMatrixCount; pageColumnCounter++) {
            int pageIndex1 = pageRowCounter * smallMatrixCount + pageColumnCounter;
            int pageIndex2 = pageColumnCounter * smallMatrixCount + pageRowCounter;

            if(pageRowCounter == pageColumnCounter) {
                Cursor cursor(matrixName, pageIndex1, true);
                vector<vector<int>> subMatrix = cursor.getPage();
                vector<vector<int>> subMatrix_t = transpose(subMatrix);
                vector<vector<int>> subMatrix_r = compute(subMatrix, subMatrix_t);
                bufferManager.writePage(resultantMatrixName, pageIndex1, subMatrix_r, subMatrix_r.size());
            }
            else {
                Cursor cursor1(matrixName, pageIndex1, true);
                Cursor cursor2(matrixName, pageIndex2, true);

                vector<vector<int>> subMatrix1 = cursor1.getPage();
                vector<vector<int>> subMatrix2 = cursor2.getPage();

                vector<vector<int>> subMatrix1_t = transpose(subMatrix1);
                vector<vector<int>> subMatrix2_t = transpose(subMatrix2);

                vector<vector<int>> subMatrix1_r = compute(subMatrix1, subMatrix2_t);
                vector<vector<int>> subMatrix2_r = compute(subMatrix2, subMatrix1_t);

                bufferManager.writePage(resultantMatrixName, pageIndex1, subMatrix1_r, subMatrix1_r.size());
                bufferManager.writePage(resultantMatrixName, pageIndex2, subMatrix2_r, subMatrix2_r.size());
            }
        }
    }

    matrixCatalogue.insertMatrix(resultantMatrix);

    cout << "Number of blocks read: " << BLOCKS_READ << endl;
    cout << "Number of blocks written:: " << BLOCKS_WRITTEN << endl;
    cout << "Number of blocks accessed: " << BLOCKS_READ + BLOCKS_WRITTEN << endl;
}