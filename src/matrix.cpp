#include "global.h"

Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        this->extractMatrixMetadata(line);
        if (this->blockify())
            return true;
    }
    fin.close();
    return false;
}

void Matrix::extractMatrixMetadata(string firstLine)
{
    string word;
    int matrixSize = 0;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        matrixSize++;
    }
    this->matrixSize = matrixSize;
    this->smallMatrixSize = (unsigned int)sqrt((BLOCK_SIZE * 1024) / sizeof(int));
    this->smallMatrixCount = (unsigned int)((this->matrixSize + this->smallMatrixSize - 1) / (this->smallMatrixSize));
}

bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    if (this->matrixSize == 0)
        return false;

    ifstream fin(this->sourceFileName, ios::in);

    string line, word;

    vector<vector<vector<int>>> elementsInPages(this->smallMatrixCount, vector<vector<int>>(this->smallMatrixSize, vector<int>(this->smallMatrixSize, -1)));

    int rowCounter = 0;
    int pageRowCounter = 0;

    while (getline(fin, line))
    {
        stringstream s(line);

        for (int columnCounter = 0; columnCounter < this->matrixSize; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            int element = stoi(word);
            elementsInPages[columnCounter / this->smallMatrixSize][rowCounter][columnCounter % this->smallMatrixSize] = element;
        }
        rowCounter++;
        if (rowCounter == this->smallMatrixSize)
        {
            for (int pageColumnCounter = 0; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++)
            {
                matrixBufferManager.writePage(this->matrixName, pageRowCounter, pageColumnCounter, elementsInPages[pageColumnCounter], this->matrixSize, this->smallMatrixSize, this->smallMatrixCount);
            }
            rowCounter = 0;
            pageRowCounter++;
            fill(elementsInPages.begin(), elementsInPages.end(), vector<vector<int>>(this->smallMatrixSize, vector<int>(this->smallMatrixSize, -1)));
        }
    }

    if (rowCounter > 0)
    {
        for (int pageColumnCounter = 0; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++)
        {
            matrixBufferManager.writePage(this->matrixName, pageRowCounter, pageColumnCounter, elementsInPages[pageColumnCounter], this->matrixSize, this->smallMatrixSize, this->smallMatrixCount);
        }
        rowCounter = 0;
        pageRowCounter++;
        fill(elementsInPages.begin(), elementsInPages.end(), vector<vector<int>>(this->smallMatrixSize, vector<int>(this->smallMatrixSize, -1)));
    }

    return true;
}

void Matrix::print()
{
    logger.log("Matrix::print");

    int rowsToWrite = min(PRINT_COUNT, this->matrixSize);
    int columnsToWrite = min(PRINT_COUNT, this->matrixSize);
    vector<vector<vector<int>>> elementsInPages(this->smallMatrixCount, vector<vector<int>>(this->smallMatrixSize, vector<int>(this->smallMatrixSize, -1)));
    
    for(int pageRowCounter = 0; pageRowCounter < this->smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = 0; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++) {
            MatrixPage matrixPage = matrixBufferManager.getPage(this->matrixName, pageRowCounter, pageColumnCounter);
            elementsInPages[pageColumnCounter] = matrixPage.getPage();
        }
        this->writeRows(elementsInPages, pageRowCounter, rowsToWrite, columnsToWrite, cout);
    }
}

void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if (!this->isPermanent())
        matrixBufferManager.deleteFile(this->sourceFileName);

    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);


    int rowsToWrite = this->matrixSize;
    int columnsToWrite = this->matrixSize;

    vector<vector<vector<int>>> elementsInPages(this->smallMatrixCount, vector<vector<int>>(this->smallMatrixSize, vector<int>(this->smallMatrixSize, -1)));
    
    for(int pageRowCounter = 0; pageRowCounter < this->smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = 0; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++) {
            MatrixPage matrixPage = matrixBufferManager.getPage(this->matrixName, pageRowCounter, pageColumnCounter);
            elementsInPages[pageColumnCounter] = matrixPage.getPage();
        }
        this->writeRows(elementsInPages, pageRowCounter, rowsToWrite, columnsToWrite, fout);
    }

    fout.close();
}

bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

void Matrix::rename(string newMatrixName) {
    logger.log("Matrix::~rename");

    for(int pageRowCounter = 0; pageRowCounter < this->smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = 0; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++) {
            matrixBufferManager.renameFile(this->matrixName, newMatrixName, pageRowCounter, pageColumnCounter);
        }
    }

    this->matrixName = newMatrixName;
    this->sourceFileName = "../data/" + newMatrixName + ".csv";
}

void Matrix::transpose(vector<vector<int>>& grid) {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = i + 1; j < grid.size(); j++) {
            swap(grid[i][j], grid[j][i]);
        }
    }
}

void Matrix::transposeMatrix()  {
    logger.log("Matrix::transposeMatrix");
    for(int pageRowCounter = 0; pageRowCounter < this->smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = pageRowCounter; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++) {
            if(pageRowCounter == pageColumnCounter) {
                MatrixPage matrixPage = matrixBufferManager.getPage(this->matrixName, pageRowCounter, pageColumnCounter);
                transpose(matrixPage.elementsInPage);
                matrixBufferManager.writePage(this->matrixName, pageRowCounter, pageColumnCounter, matrixPage.elementsInPage, this->matrixSize, this->smallMatrixSize, this->smallMatrixCount);
                matrixBufferManager.deleteFromPool(matrixPage.pageName);
            }
            else {
                MatrixPage matrixPage1 = matrixBufferManager.getPage(this->matrixName, pageRowCounter, pageColumnCounter);
                MatrixPage matrixPage2 = matrixBufferManager.getPage(this->matrixName, pageColumnCounter, pageRowCounter);
                transpose(matrixPage1.elementsInPage);
                transpose(matrixPage2.elementsInPage);
                matrixPage1.elementsInPage.swap(matrixPage2.elementsInPage);
                matrixBufferManager.writePage(this->matrixName, pageRowCounter, pageColumnCounter, matrixPage1.elementsInPage, this->matrixSize, this->smallMatrixSize, this->smallMatrixCount);
                matrixBufferManager.writePage(this->matrixName, pageColumnCounter, pageRowCounter, matrixPage2.elementsInPage, this->matrixSize, this->smallMatrixSize, this->smallMatrixCount);
                matrixBufferManager.deleteFromPool(matrixPage1.pageName);
                matrixBufferManager.deleteFromPool(matrixPage2.pageName);
            }
        }
    }
}

bool Matrix::isSymmetric(vector<vector<int>>& grid) {
    for (int i = 0; i < grid.size(); i++) {
        for (int j = i + 1; j < grid.size(); j++) {
            if(grid[i][j] != grid[j][i]) return false;
        }
    }
    return true;
}

bool Matrix::isSymmetric() {
    logger.log("Matrix::isSymmetric");
    for(int pageRowCounter = 0; pageRowCounter < this->smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = pageRowCounter; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++) {
            if(pageRowCounter == pageColumnCounter) {
                MatrixPage matrixPage = matrixBufferManager.getPage(this->matrixName, pageRowCounter, pageColumnCounter);
                if(!isSymmetric(matrixPage.elementsInPage)) return false;
            }
            else {
                MatrixPage matrixPage1 = matrixBufferManager.getPage(this->matrixName, pageRowCounter, pageColumnCounter);
                MatrixPage matrixPage2 = matrixBufferManager.getPage(this->matrixName, pageColumnCounter, pageRowCounter);
                vector<vector<int>> grid1 = matrixPage1.elementsInPage;
                vector<vector<int>> grid2 = matrixPage2.elementsInPage;
                transpose(grid1);
                if(grid1 != grid2) return false;
            }
        }
    }
    return true;
}

void Matrix::compute()  {
    ;
}

void Matrix::unload()
{
    logger.log("Matrix::~unload");
    for(int pageRowCounter = 0; pageRowCounter < this->smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = 0; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++) {
            matrixBufferManager.deleteFile(this->matrixName, pageRowCounter, pageColumnCounter);
        }
    }
    if (!isPermanent())
        matrixBufferManager.deleteFile(this->sourceFileName);
}
