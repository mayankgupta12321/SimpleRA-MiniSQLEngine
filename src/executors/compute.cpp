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

vector<vector<int>> transpose(vector<vector<int>> grid) {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = i + 1; j < grid.size(); j++) {
            swap(grid[i][j], grid[j][i]);
        }
    }
    return grid;
}

vector<vector<int>> compute(vector<vector<int>> grid, vector<vector<int>> grid_T) {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid.size(); j++) {
            grid[i][j] -= grid_T[i][j];
        }
    }
    return grid;
}

void executeCOMPUTE()
{
    logger.log("executeCOMPUTE");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.computeRelationName);
    string matrixName = matrix->matrixName;
    unsigned int matrixSize = matrix->matrixSize;
    unsigned int smallMatrixSize = matrix->smallMatrixSize;
    unsigned int smallMatrixCount = matrix->smallMatrixCount;

    string resultantMatrixName = matrixName + "_RESULT";
    Matrix *resultantMatrix = new Matrix(resultantMatrixName);
    resultantMatrix->matrixSize = matrixSize;
    resultantMatrix->smallMatrixSize = smallMatrixSize;
    resultantMatrix->smallMatrixCount = smallMatrixCount;
    
    for(int pageRowCounter = 0; pageRowCounter < smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = pageRowCounter; pageColumnCounter < smallMatrixCount; pageColumnCounter++) {
            if(pageRowCounter == pageColumnCounter) {
                MatrixPage matrixPage = matrixBufferManager.getPage(matrixName, pageRowCounter, pageColumnCounter);
                vector<vector<int>> grid = matrixPage.elementsInPage;
                vector<vector<int>> grid_T = transpose(grid);
                vector<vector<int>> grid_R = compute(grid, grid_T);
                matrixBufferManager.writePage(resultantMatrixName, pageRowCounter, pageColumnCounter, grid_R, matrixSize, smallMatrixSize, smallMatrixCount);
            }
            else {
                MatrixPage matrixPage1 = matrixBufferManager.getPage(matrixName, pageRowCounter, pageColumnCounter);
                MatrixPage matrixPage2 = matrixBufferManager.getPage(matrixName, pageColumnCounter, pageRowCounter);
                vector<vector<int>> grid1 = matrixPage1.elementsInPage;
                vector<vector<int>> grid2 = matrixPage2.elementsInPage;
                vector<vector<int>> grid1_T = transpose(grid1);
                vector<vector<int>> grid2_T = transpose(grid2);
                vector<vector<int>> grid1_R = compute(grid1, grid2_T);
                vector<vector<int>> grid2_R = compute(grid2, grid1_T);

                matrixBufferManager.writePage(resultantMatrixName, pageRowCounter, pageColumnCounter, grid1_R, matrixSize, smallMatrixSize, smallMatrixCount);
                matrixBufferManager.writePage(resultantMatrixName, pageColumnCounter, pageRowCounter, grid2_R, matrixSize, smallMatrixSize, smallMatrixCount);
            }
        }
    }

    matrixCatalogue.insertMatrix(resultantMatrix);

    return;
}