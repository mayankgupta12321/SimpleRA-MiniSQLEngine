#include "global.h"

MatrixBufferManager::MatrixBufferManager()
{
    logger.log("MatrixBufferManager::MatrixBufferManager");
}

MatrixPage MatrixBufferManager::getPage(string matrixName, int pageRowIndex, int pageColumnIndex)
{
    logger.log("MatrixBufferManager::MatrixBufferManager");
    string pageName = "../data/temp/" + matrixName + "_Page_" + to_string(pageRowIndex) + "_" + to_string(pageColumnIndex);
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(matrixName, pageRowIndex, pageColumnIndex);
}


bool MatrixBufferManager::inPool(string pageName)
{
    logger.log("MatrixBufferManager::inPool");
    for (auto page : this->pages)
    {
        if (pageName == page.pageName)
            return true;
    }
    return false;
}


MatrixPage MatrixBufferManager::getFromPool(string pageName)
{
    logger.log("MatrixBufferManager::getFromPool");
    for (auto page : this->pages)
        if (pageName == page.pageName)
            return page;
}

MatrixPage MatrixBufferManager::insertIntoPool(string matrixName, int pageRowIndex, int pageColumnIndex)
{
    logger.log("MatrixBufferManager::insertIntoPool");
    MatrixPage page(matrixName, pageRowIndex, pageColumnIndex);
    if (this->pages.size() >= BLOCK_COUNT)
        pages.pop_front();
    pages.push_back(page);
    return page;
}

void MatrixBufferManager::writePage(string matrixName, int pageRowIndex, int pageColumnIndex, vector<vector<int>> elementsInPage, int matrixSize, int smallMatrixSize, int smallMatrixCount)
{
    logger.log("BufferManager::writePage");
    MatrixPage page(matrixName, pageRowIndex, pageColumnIndex, elementsInPage, matrixSize, smallMatrixSize, smallMatrixCount);
    page.writePage();
}

void MatrixBufferManager::deleteFromPool(string pageName) {
    logger.log("MatrixBufferManager::renameInPool");
    int sz = pages.size();
    while(sz--) {
        if(pages.front().pageName != pageName) {
            pages.push_back(pages.front());
        }
        pages.pop_front();
    }
}

void MatrixBufferManager::renameFile(string curFileName, string newFileName)
{   
    if(rename(curFileName.c_str(), newFileName.c_str()))
        logger.log("MatrixBufferManager::renameFile: Err");
    else logger.log("MatrixBufferManager::renameFile: Success");
    
    if (this->inPool(curFileName)) {
        return this->deleteFromPool(curFileName);
    }
}

void MatrixBufferManager::renameFile(string curMatrixName, string newMatrixName, int pageRowIndex, int pageColumnIndex) {
    logger.log("MatrixBufferManager::renameFile");
    string curFileName = "../data/temp/" + curMatrixName + "_Page_" + to_string(pageRowIndex) + "_" + to_string(pageColumnIndex);
    string newFileName = "../data/temp/" + newMatrixName + "_Page_" + to_string(pageRowIndex) + "_" + to_string(pageColumnIndex);
    this->renameFile(curFileName, newFileName);
}

void MatrixBufferManager::deleteFile(string fileName)
{    
    if(remove(fileName.c_str()))
        logger.log("MatrixBufferManager::deleteFile: Err");
    else logger.log("MatrixBufferManager::deleteFile: Success");
}


void MatrixBufferManager::deleteFile(string matrixName, int pageRowIndex, int pageColumnIndex)
{
    logger.log("MatrixBufferManager::deleteFile");
    string fileName = "../data/temp/" + matrixName + "_Page_" + to_string(pageRowIndex) + "_" + to_string(pageColumnIndex);
    this->deleteFile(fileName);
}