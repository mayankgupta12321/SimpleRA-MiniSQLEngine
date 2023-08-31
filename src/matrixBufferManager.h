#include "matrixPage.h"

class MatrixBufferManager{
    deque<MatrixPage> pages;
    bool inPool(string pageName);
    MatrixPage getFromPool(string pageName);
    MatrixPage insertIntoPool(string matrixName, int pageRowIndex, int pageColumnIndex);
    public:
    
    MatrixBufferManager();
    MatrixPage getPage(string matrixName, int pageRowIndex, int pageColumnIndex);
    void writePage(string matrixName, int pageRowIndex, int pageColumnIndex, vector<vector<int>> elementsInPage, int matrixSize, int smallMatrixSize, int smallMatrixCount);
    void deleteFromPool(string pageName);
    void renameFile(string curMatrixName, string newMatrixName, int pageRowIndex, int pageColumnIndex);
    void renameFile(string curFileName, string newFileName);
    void deleteFile(string matrixName, int pageRowIndex, int pageColumnIndex);
    void deleteFile(string fileName);
};

    