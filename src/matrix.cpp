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

    

    int rowCounter = 0;
    vector<vector<vector<int>>> elementsInPages(this->smallMatrixCount);

    while (getline(fin, line))
    {
        stringstream s(line);

        for (int columnCounter = 0; columnCounter < this->matrixSize; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            int element = stoi(word);

            if(elementsInPages[columnCounter / this->smallMatrixSize].size() == rowCounter){
                elementsInPages[columnCounter / this->smallMatrixSize].push_back({});
            }

            if(elementsInPages[columnCounter / this->smallMatrixSize][rowCounter].size() == columnCounter % this->smallMatrixSize){
                elementsInPages[columnCounter / this->smallMatrixSize][rowCounter].push_back({});
            }

            elementsInPages[columnCounter / this->smallMatrixSize][rowCounter][columnCounter % this->smallMatrixSize] = element;
        }

        rowCounter++;
        if (rowCounter == this->smallMatrixSize)
        {
            for (int pageColumnCounter = 0; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++)
            {
                bufferManager.writePage(this->matrixName, this->blockCount, elementsInPages[pageColumnCounter], rowCounter);
                this->blockCount++;
            }
            rowCounter = 0;
            elementsInPages.clear();
            elementsInPages.resize(this->smallMatrixCount);
        }
    }

    if (rowCounter > 0)
    {
        for (int pageColumnCounter = 0; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++)
        {
            bufferManager.writePage(this->matrixName, this->blockCount, elementsInPages[pageColumnCounter], rowCounter);
            this->blockCount++;
        }
        rowCounter = 0;
    }

    return true;
}

void Matrix::print()
{
    logger.log("Matrix::print");

    int rowsToWrite = min(PRINT_COUNT, this->matrixSize);
    int columnsToWrite = min(PRINT_COUNT, this->matrixSize);

    Cursor cursor(this->matrixName, 0, true);
    vector<int> subMatrixRow;
    

    int rowIndex = 0;
    for(int rowBlockCounter = 0; rowBlockCounter < smallMatrixCount; rowBlockCounter++) {

        int rowCount = (rowBlockCounter == smallMatrixCount - 1) ? matrixSize - rowBlockCounter * smallMatrixSize: smallMatrixSize;
        int columnCount = this->matrixSize;

        vector<vector<int>> matrix(rowCount);

        int columnBlockCounter = 0;
        while(columnBlockCounter < smallMatrixCount) {
            subMatrixRow = cursor.getNextPageRow();
            matrix[rowIndex].insert(matrix[rowIndex].end(), subMatrixRow.begin(), subMatrixRow.end());
            rowIndex = (rowIndex + 1) % rowCount;
            if(rowIndex == 0){
                columnBlockCounter++;
            }
        }

        for(int rowCounter = 0; rowCounter < rowCount; rowCounter++){
            this->writeRow(matrix[rowCounter], cout, columnsToWrite);
            rowsToWrite--;
            if(rowsToWrite == 0) return;
        }

        rowIndex = 0;
        columnBlockCounter = 0;
    }
}

void Matrix::getNextPage(Cursor *cursor) {
    logger.log("Matrix::getNegetNextPagext");
    if (cursor->pageIndex < this->blockCount - 1) {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if (!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);

    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    Cursor cursor(this->matrixName, 0, true);
    vector<int> subMatrixRow;
    

    int rowIndex = 0;
    for(int rowBlockCounter = 0; rowBlockCounter < smallMatrixCount; rowBlockCounter++) {

        int rowCount = (rowBlockCounter == smallMatrixCount - 1) ? matrixSize - rowBlockCounter * smallMatrixSize: smallMatrixSize;
        int columnCount = this->matrixSize;

        vector<vector<int>> matrix(rowCount);

        int columnBlockCounter = 0;
        while(columnBlockCounter < smallMatrixCount) {
            subMatrixRow = cursor.getNextPageRow();
            matrix[rowIndex].insert(matrix[rowIndex].end(), subMatrixRow.begin(), subMatrixRow.end());
            rowIndex = (rowIndex + 1) % rowCount;
            if(rowIndex == 0){
                columnBlockCounter++;
            }
        }

        for(int rowCounter = 0; rowCounter < rowCount; rowCounter++){
            this->writeRow(matrix[rowCounter], fout, matrix[rowCounter].size(), true);
        }

        rowIndex = 0;
        columnBlockCounter = 0;
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

    for (int blockCounter = 0; blockCounter < this->blockCount; blockCounter++) {
        bufferManager.renameFile(this->matrixName, newMatrixName, blockCounter);
    }

    this->matrixName = newMatrixName;
    this->sourceFileName = "../data/" + newMatrixName + ".csv";
}

vector<vector<int>> Matrix::transpose(vector<vector<int>> &grid) {
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

void Matrix::transposeMatrix()  {
    logger.log("Matrix::transposeMatrix");

    
    for(int pageRowCounter = 0; pageRowCounter < this->smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = pageRowCounter; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++) {
            int pageIndex1 = pageRowCounter * smallMatrixCount + pageColumnCounter;
            int pageIndex2 = pageColumnCounter * smallMatrixCount + pageRowCounter;

            if(pageRowCounter == pageColumnCounter) {
                Cursor cursor(this->matrixName, pageIndex1, true);
                vector<vector<int>> subMatrix = cursor.getPage();
                subMatrix = transpose(subMatrix);
                bufferManager.writePage(this->matrixName, pageIndex1, subMatrix, subMatrix.size());
                bufferManager.deleteFromPool(cursor.page.pageName);
            }
            else {
                Cursor cursor1(this->matrixName, pageIndex1, true);
                Cursor cursor2(this->matrixName, pageIndex2, true);

                vector<vector<int>> subMatrix1 = cursor1.getPage();
                vector<vector<int>> subMatrix2 = cursor2.getPage();

                subMatrix1 = transpose(subMatrix1);
                subMatrix2 = transpose(subMatrix2);

                bufferManager.writePage(this->matrixName, pageIndex1, subMatrix2, subMatrix2.size());
                bufferManager.writePage(this->matrixName, pageIndex2, subMatrix1, subMatrix1.size());

                bufferManager.deleteFromPool(cursor1.page.pageName);
                bufferManager.deleteFromPool(cursor2.page.pageName);
            }
        }
    }
}

bool Matrix::isSymmetric() {
    logger.log("Matrix::isSymmetric");
    for(int pageRowCounter = 0; pageRowCounter < this->smallMatrixCount; pageRowCounter++) {
        for(int pageColumnCounter = pageRowCounter; pageColumnCounter < this->smallMatrixCount; pageColumnCounter++) {
            int pageIndex1 = pageRowCounter * smallMatrixCount + pageColumnCounter;
            int pageIndex2 = pageColumnCounter * smallMatrixCount + pageRowCounter;

            if(pageRowCounter == pageColumnCounter) {
                Cursor cursor(this->matrixName, pageIndex1, true);
                vector<vector<int>> subMatrix = cursor.getPage();
                vector<vector<int>> subMatrix_t = transpose(subMatrix);
                if(subMatrix_t != subMatrix) return false;
            }
            else {
                Cursor cursor1(this->matrixName, pageIndex1, true);
                Cursor cursor2(this->matrixName, pageIndex2, true);

                vector<vector<int>> subMatrix1 = cursor1.getPage();
                vector<vector<int>> subMatrix2 = cursor2.getPage();

                vector<vector<int>> subMatrix1_t = transpose(subMatrix1);
                if(subMatrix1_t != subMatrix2) return false;
            }
        }
    }
    return true;
}

void Matrix::unload()
{
    logger.log("Matrix::~unload");
    
    for (int blockCounter = 0; blockCounter < this->blockCount; blockCounter++)
        bufferManager.deleteFile(this->matrixName, blockCounter);

    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}
